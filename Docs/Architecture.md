# Sign2Voice Architecture

## Layer Diagram

```
┌──────────────────────────────────────────────────────────┐
│                     APPLICATION                           │
│   gesture_app (sliding window, cooldown, MP3 trigger)     │
│   menu (8-page LVGL UI, key navigation)                  │
│   ui_overlay (top bar + bottom panel camera overlay)      │
│   camera_band_refresh (partial SPI DMA LCD refresh)       │
├──────────────────────────────────────────────────────────┤
│                       SERVICE                             │
│   ai_service     — X-CUBE-AI wrapper, model-agnostic     │
│   camera_service — OV2640 + DCMI + frame capture         │
│   audio_service  — MP3-TF-16P playback, dedup + cooldown │
│   ui_service     — LVGL init, display refresh            │
│   watchdog       — IWDG (~4s timeout, register access)   │
├──────────────────────────────────────────────────────────┤
│                       CONFIG                              │
│   config         — SystemConfig struct, get/set/load/save│
│   flash_storage  — Bank2 Sector7 R/W (SRAM resident)     │
│   error_code     — Unified ErrorCode enum                 │
│   log            — Compile-time LOG_INFO/WARN/ERROR       │
├──────────────────────────────────────────────────────────┤
│                       DRIVER                              │
│   OV2640  ILI9341  MP3(YX5200)  KEY  LED  ADC  IWDG     │
├──────────────────────────────────────────────────────────┤
│                        HAL                                │
│               STM32H7xx HAL Library                       │
└──────────────────────────────────────────────────────────┘
```

## Module Index (v2.2)

| Layer | Module | Path | Key API |
|---|---|---|---|
| App | gesture_app | App/gesture_app.c | `init()`, `on_result()`, `reset()` |
| App | app_init | App/app_init.c | `app_init()` — unified hardware + service bootstrap |
| App | menu | Core/Src/menu.c | `menu_init()`, `menu_process_key()`, `menu_get_*()` |
| App | ui_overlay | Core/Src/ui_overlay.c | `ui_overlay_init()`, `ui_overlay_set_*()`, `ui_overlay_update()` |
| Service | ai_service | Service/ai_service.c | `init/start/stop/deinit`, `run()`, `get_class/prob/inf_ms()` |
| Service | camera_service | Service/camera_service.c | `init/start/stop/deinit`, `frame_ready()`, `get_rgb888()` |
| Service | audio_service | Service/audio_service.c | `init/start/stop/deinit`, `set_volume()`, `play_class()`, `reset()` |
| Service | ui_service | Service/ui_service.c | `init/start/stop/deinit`, `tick()`, `update()`, `set_*()` |
| Service | watchdog | Service/watchdog.c | `init()` (task context), `feed()` |
| Config | config | Config/config.c | `init/load/save/reset`, `get/set_*()` |
| Storage | flash_storage | Storage/flash_storage.c | `read()`, `write()` (SRAM) |
| Common | error_code | Common/error_code.h | `ErrorCode` enum with `ERR_OK` through `ERR_NULL_POINTER` |
| Debug | log | Debug/log.h | `LOG_LEVEL_NONE/ERROR/WARN/INFO`, `LOG_*()` macros |

## RTOS Task Model

Three tasks decouple camera capture, AI inference, and display rendering:

```
┌─────────────┐    frameQueue    ┌──────────┐   resultQueue   ┌─────────────┐
│ display_lcd │ ────msgQ(4)────→ │   inf    │ ──mailQ(2)────→ │ display_lcd │
│ Priority: 3 │                  │Prior: 3  │                 │  (same task)│
│ Stack: 16KB │                  │Stack:8KB │                 │             │
└─────────────┘                  └──────────┘                 └─────────────┘
       │                              │                              │
       ▼                              ▼                              ▼
  camera_service                ai_service                  gesture_app
  lv_timer_handler              camera_service               audio_service
  menu_process_key              (preprocess)                 ui_service
  watchdog_feed                                              
```

**Key design decisions**:
- `inf` task blocks on frame queue (no polling → zero CPU waste when idle)
- Result delivered via mail queue (allocated in `inf`, freed in `display_lcd`)
- Both tasks at same priority (round-robin time-sliced by FreeRTOS)
- Sliding window + cooldown provide immunity to transient AI misclassification

## Data Flow

```
OV2640 → DCMI DMA → RGB_DATA[320][240] (AXI SRAM, 150KB)
    │
    ▼ [display_lcd task]
SCB_InvalidateDCache → camera_band_refresh() → LCD via SPI DMA
    │                          │
    ▼                          ▼
RGB565→64×64 float tensor    LVGL top bar + bottom panel
    │                          (partial refresh: rows 0-19, 240-319)
    ▼ [inf task]
ai_service_run() → X-CUBE-AI INT8 CMSIS-NN inference
    │
    ▼ [display_lcd task]
gesture_app_on_result() → sliding window → cooldown → audio_service_play_class()
```

## Memory Regions (v2.2)

| Region | Address | Size | Used | Critical Contents |
|---|---|---|---|---|
| DTCM | 0x20000000 | 128KB | 76KB | FreeRTOS heap (64KB), task stacks, MSP |
| AXI SRAM | 0x24000000 | 512KB | 460KB | RGB_DATA×2 (300KB), AI input (48KB), activations (77KB) |
| SRAM1+2 | 0x30000000 | 256KB | 93KB | LVGL draw buf (42KB), DCMI DMA, `.RAM_D2` code |
| Flash | 0x08000000 | 2MB | ~700KB | Code (565KB), weights (139KB), config (32B in Sector 7) |

**Bottleneck**: AXI SRAM at 90%. RGB_DATA uses 300KB (two 150KB buffers for camera frame + LCD DMA).

## Performance (v2.2)

| Metric | v2.1 | v2.2 |
|---|---|---|
| AI Inference | ~135 ms | **123 ms** |
| FPS | ~6 | ~6 |
| Flash usage | ~400 KB | ~700 KB (with CMSIS-NN lib) |
| RAM usage | ~600 KB | ~630 KB |
| IWDG | — | 4s timeout |

## Version History

| Version | Key Changes |
|---|---|
| v1.0 | Functional prototype: camera + AI + MP3 + LVGL |
| v2.0 | Layered architecture: App/Service/Driver/HAL |
| v2.1 | Config persistence, ErrorCode, logging, cooldown, Flash bank fix |
| v2.2 | CMSIS-NN, IWDG watchdog, benchmark |
