# Sign2Voice — Real-time Sign Language Recognition on STM32H743

[![Platform](https://img.shields.io/badge/platform-STM32H743VIT6-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32h743vi.html)
[![Framework](https://img.shields.io/badge/framework-FreeRTOS%20%2B%20LVGL%20v9-orange)](https://lvgl.io/)
[![AI](https://img.shields.io/badge/AI-X--CUBE--AI%20INT8-green)](https://www.st.com/en/embedded-software/x-cube-ai.html)
[![License](https://img.shields.io/badge/license-MIT-lightgrey)](LICENSE)

> **Real-time, fully offline hand gesture recognition on a $15 MCU.**
> Point a camera at your hand → the device speaks the gesture aloud via an MP3 module.

<p align="center">
  <img src="docs/architecture.png" alt="System Overview" width="700"/>
</p>

---

## Features

- **5 gestures**: Fist, Like, OK, Palm, No-Gesture (mute)
- **Real-time**: 6-7 FPS end-to-end (135ms INT8 inference)
- **Fully offline**: No Wi-Fi, no cloud — everything runs on STM32H743
- **Voice feedback**: MP3-TF-16P module speaks recognized gestures
- **Interactive menu**: LVGL v9 GUI with settings (volume, threshold, window size)
- **Custom model ready**: Swap any X-CUBE-AI compatible `.tflite` model

## Hardware

| Component | Part |
|---|---|
| MCU | STM32H743VIT6 (Cortex-M7 @400MHz, 2MB Flash, 1MB SRAM) |
| Camera | OV2640 (200W pixel, DCMI interface) |
| Display | ILI9341 SPI LCD (320×240, RGB565) |
| Audio | MP3-TF-16P (YX5200 UART, microSD) |
| Debug | ST-Link V2 |

## Architecture (v2.1)

```
┌─────────────────────────────────────────────────┐
│                  APPLICATION                     │
│  ┌──────────┐ ┌──────────┐ ┌──────┐ ┌────────┐ │
│  │  Menu    │ │ Gesture  │ │Settings│ │ Status │ │
│  └──────────┘ └──────────┘ └──────┘ └────────┘ │
├─────────────────────────────────────────────────┤
│                    SERVICE                       │
│  ┌──────────┐ ┌──────────┐ ┌──────┐ ┌────────┐ │
│  │ AI Svc   │ │ Camera   │ │Audio │ │  UI    │ │
│  │          │ │ Svc      │ │Svc   │ │ Svc    │ │
│  └──────────┘ └──────────┘ └──────┘ └────────┘ │
├─────────────────────────────────────────────────┤
│                    CONFIG                        │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────┐ │
│  │    Config    │ │ Flash Storage│ │ ErrorCode│ │
│  │ (SystemCfg)  │ │ (Bank2 Sct7) │ │  (enum)  │ │
│  └──────────────┘ └──────────────┘ └──────────┘ │
├─────────────────────────────────────────────────┤
│                    DRIVER                        │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐  │
│  │OV2640│ │ILI9341│ │ MP3  │ │ KEY  │ │ ADC  │  │
│  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘  │
├─────────────────────────────────────────────────┤
│                      HAL                         │
│              STM32H7xx HAL Library               │
└─────────────────────────────────────────────────┘
```

### Project Structure

```
Sign2Voice/
├── App/                    # Application layer (v2.1)
│   ├── gesture_app.c/h     #   Recognition pipeline, cooldown, dedup
│   └── app_init.c/h        #   Unified hardware + service init
├── Service/                # Service layer (v2.1)
│   ├── ai_service.c/h      #   X-CUBE-AI wrapper, model-agnostic API
│   ├── camera_service.c/h  #   OV2640 + DCMI + frame capture
│   ├── audio_service.c/h   #   MP3 playback, class→track mapping
│   └── ui_service.c/h      #   LVGL overlay + display refresh
├── Config/                 # Configuration (v2.1)
│   └── config.c/h          #   SystemConfig load/save/reset
├── Storage/                # Flash persistence (v2.1)
│   └── flash_storage.c/h   #   Bank2 Sector7 R/W (SRAM-resident)
├── Common/                 # Shared (v2.1)
│   └── error_code.h        #   Unified ErrorCode enum
├── Debug/                  # Logging (v2.1)
│   └── log.h/c             #   Compile-time controlled LOG macros
├── Docs/                   # Documentation (v2.1)
│   ├── Architecture.md     #   Layer diagram, task model, data flow
│   ├── Configuration.md    #   Config struct, flash storage, debugging
│   ├── ErrorHandling.md    #   Error codes, fault handlers
│   ├── DebugGuide.md       #   Log levels, UART output, fault dump
│   └── MemoryLayout.md     #   RAM/Flash regions, scatter file
├── Core/
│   ├── Src/
│   │   ├── freertos.c      #   3 RTOS tasks (display, inference, audio)
│   │   ├── main.c          #   System entry, clock config
│   │   ├── menu.c/h        #   Multi-page settings menu
│   │   └── ui_overlay.c/h  #   LVGL camera overlay (top bar + bottom panel)
│   └── Inc/                #   FreeRTOSConfig, lv_conf, HAL conf
├── HARDWARE/TIMER/         #   Drivers (OV2640, ILI9341, MP3, KEY, LED...)
├── X-CUBE-AI/App/          #   AI model (gesture: INT8, 5-class)
└── MDK-ARM/                #   Keil / EIDE project files
```

## RTOS Task Model

```mermaid
graph TD
    DCMI[DCMI DMA<br/>IRQ] -->|writes| RGB_DATA[RGB_DATA<br/>AXI SRAM]
    DISPLAY[display_lcd<br/>4096 stack] -->|frame ready?| CAM[camera_service]
    CAM -->|osMessagePut| FRAME_Q[frameQueue]
    INF[info<br/>2048 stack] -->|osMessageGet| FRAME_Q
    INF -->|preprocess + infer| AI[ai_service]
    AI -->|osMailPut| RESULT_Q[resultQueue]
    DISPLAY -->|osMailGet| RESULT_Q
    DISPLAY -->|gesture_app_on_result| SLIDE[Sliding Window]
    SLIDE -->|confirmed| MP3[audio_service]
    DISPLAY -->|lv_timer_handler| LVGL[LVGL UI]
```

## Memory Layout

| Region | Address | Size | Usage |
|---|---|---|---|
| DTCM | 0x20000000 | 128KB | Stack, FreeRTOS heap, critical BSS |
| AXI SRAM | 0x24000000 | 512KB | `RGB_DATA` (150KB), `RGB_DATA_Contiguous`, AI buffers |
| SRAM1+2 | 0x30000000 | 256KB | DCMI DMA double-buffer, LVGL draw buffers |
| Flash | 0x08000000 | 2MB | Code + AI weights + Config (Sector7) |

## AI Model

| Property | Value |
|---|---|
| Architecture | Custom CNN (X-CUBE-AI generated) |
| Quantization | INT8 |
| Input | 64×64×3 (RGB, float) |
| Output | 5 classes (fist, like, no_gesture, ok, palm) |
| Inference time | ~135 ms |
| Flash (weights) | ~216 KB |
| RAM (activations) | ~40 KB |

See [pose estimation pipeline](MP3-TF-16P模块使用说明书/pro/) for the next-generation model training.

## Quick Start

### Prerequisites

- STM32CubeIDE or Keil MDK or [EIDE](https://github.com/github0null/eide) (VSCode)
- ST-Link V2 debugger
- Hardware: STM32H743VIT6 + OV2640 + ILI9341 + MP3-TF-16P

### Build & Flash

```bash
# Using EIDE (VSCode)
1. Open project in VSCode with EIDE extension
2. Select target "finaltest"
3. Build → Flash

# Or open MDK-ARM/finaltest.uvprojx in Keil MDK
```

### Deploy a Custom Model

1. Train your TFLite model (input: 64×64×3, output: N-class softmax)
2. Quantize to INT8 using X-CUBE-AI CLI or STM32Cube.AI
3. Replace `X-CUBE-AI/App/gesture.*` with generated files
4. Update `ai_service_get_class_count()` return value in `Service/ai_service.c`
5. Update `class_names[]` in `X-CUBE-AI/App/app_x-cube-ai.c`
6. Rebuild → Flash

## Roadmap

See [plan.md](plan.md) for the full development roadmap.

- [x] v1.0 — Functional prototype (gesture recognition + voice)
- [x] v2.0 — Software architecture refactoring (App/Service/Driver layers)
- [x] v2.1 — Reliability & fault tolerance (config persistence, watchdog, error codes, logging, docs)
- [ ] v2.2 — Performance optimization (CMSIS-NN)
- [ ] v2.3 — Documentation
- [ ] v3.0 — Custom PCB hardware

## License

MIT License © 2026

## Acknowledgments

- [X-CUBE-AI](https://www.st.com/en/embedded-software/x-cube-ai.html) — STM32 AI expansion pack
- [LVGL](https://lvgl.io/) — Light and Versatile Embedded Graphics Library
- [FreeRTOS](https://www.freertos.org/) — Real-time operating system
- COCO Keypoints dataset for pose estimation research
