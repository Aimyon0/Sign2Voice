# Sign2Voice

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-STM32H743-orange.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32h743vi.html)
[![Framework](https://img.shields.io/badge/framework-FreeRTOS%20%2B%20LVGL%20v9-green.svg)]()

**Real-time hand gesture recognition on a $20 MCU.**

Sign2Voice is an embedded TinyML system that recognizes 5 static hand gestures using a CNN running entirely on an STM32H743 microcontroller, and speaks the recognized word through an MP3 module — no cloud, no phone, no latency.

<img src="Images/demo.gif" width="600" alt="Demo GIF">

---

## Table of Contents

- [Hardware](#hardware)
- [Software Architecture](#software-architecture)
- [AI Pipeline](#ai-pipeline)
- [Performance](#performance)
- [Features](#features)
- [Folder Structure](#folder-structure)
- [Getting Started](#getting-started)
- [Demo](#demo)
- [License](#license)

---

## Hardware

| Component | Model | Interface |
|-----------|-------|-----------|
| MCU | STM32H743VIT6 (Cortex-M7 @400MHz) | — |
| Camera | OV2640 (2MP) | DCMI 8-bit + DMA |
| Display | ILI9341 2.8" 320×240 | SPI2 @25MHz (DMA) |
| Audio | MP3-TF-16P (YX5200) | USART1 9600bps |
| Input | 2× push buttons | GPIO |

### Memory Layout

| Region | Size | Usage |
|--------|------|-------|
| DTCM | 128 KB | FreeRTOS kernel, task stacks |
| AXI SRAM | 512 KB | Frame buffer (150 KB), AI runtime |
| SRAM1+2 | 256 KB | DMA buffers, LVGL display buffers |
| Flash | 2 MB | Application code + AI model weights |

See [Docs/Memory-Layout.md](Docs/Memory-Layout.md) for details.

---

## Software Architecture

```
Application  ┬─ menu.c / ui_overlay.c / camera_band_refresh.c / sliding window
Middleware   ├─ FreeRTOS (task scheduler + IPC via osMessageQ + osMailQ)
             ├─ LVGL v9 (GUI engine, partial render mode)
             └─ X-CUBE-AI (CNN inference runtime)
Drivers      ├─ OV2640 / ILI9341 / YX5200 / KEY / DCMI+DMA / ADC
HAL          └─ STM32H7 HAL + CMSIS
```

### Task Model

| Task | Priority | Stack | Role |
|------|----------|-------|------|
| `display_lcd` | Normal | 4096 words | Camera refresh, LVGL render, key scan, MP3 play |
| `info` | Normal | 2048 words | Block on frame queue → Preprocess → AI Inference → Mail result |
| `out` | Low | 1024 words | MP3 background (idle) |

See [Docs/Task-Diagram.md](Docs/Task-Diagram.md) for details.

---

## AI Pipeline

```
Camera → DCMI+DMA → Frame Buffer (AXI SRAM, 320×240 RGB565)
                    ↓ D-Cache invalidate
               Bilinear Resize to 64×64×3 RGB888 Float
                    ↓
               CNN (INT8 quantized, X-CUBE-AI)
                    ↓
               Argmax → {fist, like, no_gesture, ok, palm}
                    ↓
          Sliding Window (N=3/5/7) → MP3 Playback
```

The CNN model is trained with TensorFlow/Keras, exported as `.tflite`, then converted to optimized C code by STM32Cube.AI and deployed entirely on-device.

See [Docs/AI-Pipeline.md](Docs/AI-Pipeline.md) for details.

---

## Performance

| Metric | Value |
|--------|-------|
| MCU | STM32H743VIT6 @400 MHz |
| Camera | OV2640, 320×240@30fps |
| Display | ILI9341 SPI, 320×240 |
| RTOS | FreeRTOS |
| GUI | LVGL v9 |
| AI Engine | X-CUBE-AI |
| Model | CNN INT8 |
| Inference Time | ~135 ms |
| End-to-End Latency | ~215 ms |
| Display FPS | ~6–7 |
| Accuracy | 92.6% (5-class) |
| Gestures | Fist / Like / No-gesture / OK / Palm |
| Offline | Yes — zero cloud dependency |

---

## Features

- **100% offline** edge AI inference
- **INT8 quantized CNN** for fast, low-power inference
- **Sliding-window voting filter** eliminates false triggers
- **Dual-queue IPC** (osMessageQ + osMailQ) keeps UI fluid during inference
- **9-screen LVGL menu** with threshold, window size, and volume controls
- **Real-time FPS, inference time, CPU temperature monitoring**
- **Modular drivers** for camera, display, and audio

---

## Folder Structure

```
Sign2Voice/
├── Core/                   # Application source code
│   ├── Inc/                # Headers (lv_conf.h, FreeRTOSConfig.h, menu.h, ...)
│   └── Src/                # main.c, freertos.c, menu.c, lv_port_*.c, ...
├── Drivers/                # MCU peripheral drivers
│   ├── CMSIS/              # CMSIS core + device headers
│   └── STM32H7xx_HAL_Driver/ # HAL source
├── Hardware/               # Custom hardware drivers
│   ├── TIMER/              # OV_Frame, LCD, key, delay, led, ...
│   └── MP3/                # MP3-TF-16P (YX5200) driver
├── Middlewares/             # Third-party middleware
│   ├── FreeRTOS/           # FreeRTOS kernel + CMSIS-RTOS wrapper
│   ├── LVGL/               # LVGL v9 GUI library
│   └── ST/AI/              # STM32Cube.AI runtime + headers
├── AI/                     # AI model files (X-CUBE-AI generated)
│   ├── gesture.c/h         # Model runtime
│   ├── gesture_data.c/h    # Model data
│   └── app_x-cube-ai.c/h   # AI application wrapper
├── MDK-ARM/                # Keil MDK project files
│   ├── finaltest.uvprojx   # Keil project
│   ├── finaltest.sct       # Scatter-loading file (memory layout)
│   ├── startup_stm32h743xx.s
│   └── .eide/              # Embedded IDE (VSCode) config
├── Docs/                   # Documentation
├── Images/                 # Screenshots and photos
├── model/                  # Original .tflite model files
├── README.md
├── LICENSE
└── .gitignore
```

---

## Getting Started

### Prerequisites

- **Keil MDK** (ARM Compiler 5) or **EIDE** (VSCode extension)
- STM32H743VIT6 board + OV2640 camera + ILI9341 LCD + MP3-TF-16P
- STM32CubeMX 6.x + X-CUBE-AI 10.1.0

### Build (EIDE)

1. Open VSCode in `MDK-ARM/`
2. Install [EIDE](https://marketplace.visualstudio.com/items?itemName=CL.eide) extension
3. Click EIDE icon → "Open Project" → select `finaltest.code-workspace`
4. Build → Flash (ST-Link)

### Build (Keil MDK)

1. Open `MDK-ARM/finaltest.uvprojx`
2. Project → Build Target (F7)
3. Flash → Download (F8)

### Customize the Model

1. Train your gesture CNN using TensorFlow/Keras
2. Export as `.tflite`
3. Open STM32CubeMX → X-CUBE-AI → Add model
4. Regenerate code → restore [memory config](Docs/Memory-Layout.md) manually
5. Update `class_names[]` in `AI/app_x-cube-ai.c`
6. Place MP3 files `001.MP3`–`004.MP3` on SD card

---

## Demo

[Watch on YouTube](https://youtu.be/placeholder)

---

## License

MIT — see [LICENSE](LICENSE) for details.
