# Sign2Voice Architecture (v2.1)

## Layer Diagram

```
Application     gesture_app   menu   ui_overlay   camera_band_refresh
Service         ai_service   camera_service   audio_service   ui_service
Config          config (SystemConfig struct, Flash persistence)
Driver          OV2640   ILI9341   MP3(YX5200)   KEY   LED   ADC
HAL             STM32H7xx HAL Library
```

## Module List

| Layer | Module | Files | Role |
|---|---|---|---|
| App | gesture_app | App/gesture_app.c | Sliding-window, cooldown 2s, MP3 trigger |
| App | app_init | App/app_init.c | Hardware + service init |
| App | menu | Core/Src/menu.c | Multi-page settings (8 pages) |
| Service | ai_service | Service/ai_service.c | X-CUBE-AI wrapper |
| Service | camera_service | Service/camera_service.c | OV2640 + DCMI frame capture |
| Service | audio_service | Service/audio_service.c | MP3-TF-16P playback |
| Service | ui_service | Service/ui_service.c | LVGL display management |
| Config | config | Config/config.c | SystemConfig, get/set/load/save |
| Storage | flash_storage | Storage/flash_storage.c | Bank 2 Sector 7 R/W |
| Common | error_code | Common/error_code.h | ErrorCode enum |
| Debug | log | Debug/log.h | LOG_INFO/WARN/ERROR macros |

## RTOS Tasks

| Task | Priority | Stack | Role |
|---|---|---|---|
| display_lcd | Normal | 4096 | Camera preview, LVGL, menu, key scan |
| inf | Normal | 2048 | Frame preprocess + AI inference |
| mp3 | BelowNormal | 1024 | Idle (playback driven by display task) |

## IPC

| Queue | Direction | Type | Data |
|---|---|---|---|
| frameQueue | display → inf | osMessageQ (depth 4) | uint32_t trigger |
| resultQueue | inf → display | osMailQ (depth 2) | ui_event_t {class, prob, inf_ms} |

## Data Flow

```
OV2640 → DCMI DMA → RGB_DATA (AXI SRAM)
    ↓
camera_band_refresh() → LCD SPI DMA
    ↓
RGB565 → 64×64 RGB888 float → ai_input_data (AI_RAM)
    ↓
ai_service_run() → class + confidence
    ↓
gesture_app_on_result() → sliding window → cooldown → audio_service_play_class()
```
