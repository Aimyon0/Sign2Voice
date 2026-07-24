# Sign2Voice v2.2 — Performance Benchmark

**Hardware**: STM32H743VIT6 @400MHz  
**Date**: 2026-07-24  
**Toolchain**: ARM Compiler 5 (Keil MDK)  
**Source**: Map file (`build/finaltest/finaltest.map`)

## Real-Time Performance

| Metric | v2.1 | v2.2 (CMSIS-NN) |
|---|---|---|
| AI Inference | ~135 ms | **123 ms** |
| End-to-end FPS | ~6 | ~6 |

> Inference time measured via `HAL_GetTick()` in `ai_service_run()`. FPS bound by inference (123 ms) + preprocessing (~30 ms) + display (~10 ms) ≈ 165 ms/frame.

## AI Model

| Property | Value |
|---|---|
| Model | Custom CNN (X-CUBE-AI, ST Edge AI Core v2.2.0) |
| Quantization | INT8, per-channel (`ss/sa`) |
| Optimization | `-O time` with CMSIS-NN runtime |
| Input | 64×64×3, f32→int8 (quantize layer) |
| Output | 5-class softmax |
| MACC | 7,332,386 |
| Layers | 3× Conv2D + 3× MaxPool + 2× Dense + Softmax |
| Weights | 142,756 B (139 KB) |
| Activations | 79,024 B (77 KB) |

## Memory Budget

### Flash (2 MB total)

| Component | Size | % |
|---|---|---|
| Code + RO Data (from map) | 578,480 B (565 KB) | 28.3% |
| Reserved Config Storage | 131,072 B (128 KB) | 6.4% |
| **Free** | **~1.3 MB** | **65.3%** |

### RAM (1 MB total)

| Region | Base | Size | Used | % |
|---|---|---|---|---|
| DTCM (`RW_IRAM1`) | 0x20000000 | 128 KB | 78,096 B (76 KB) | 59.6% |
| AXI SRAM (`RW_IRAM2`) | 0x24000000 | 512 KB | 471,360 B (460 KB) | 90.0% |
| SRAM1+2 (`RW_IRAM3`) | 0x30000000 | 256 KB | 95,392 B (93 KB) | 36.4% |
| **Total** | — | 1,024 KB | 644,848 B (630 KB) | **61.5%** |

> AXI SRAM is the tightest region at 90% — dominated by `RGB_DATA` (150 KB) + `RGB_DATA_Contiguous` (150 KB) + AI buffers (~125 KB).

### Largest Allocations

| Symbol | Size | Region |
|---|---|---|
| `RGB_DATA[320][240]` | 150 KB | AXI SRAM |
| `RGB_DATA_Contiguous[]` | 150 KB | AXI SRAM |
| `disp_buf1` (LVGL) | 42 KB | SRAM1+2 |
| LVGL Memory Pool | 32 KB | AXI SRAM |
| FreeRTOS Heap | 64 KB | DTCM |
| AI Activations (`pool0`) | 77 KB | AXI SRAM |
| AI Input Buffer | 48 KB | AXI SRAM |
| `display_lcd` Task Stack | 16 KB | DTCM |
| `inf` Task Stack | 8 KB | DTCM |
| `mp3` Task Stack | 4 KB | DTCM |
| Main Stack (MSP) | 4 KB | DTCM |
| `RGB_Line_Buf[2][480]` | 3.75 KB | SRAM1+2 |

## Configuration Storage

| Item | Value |
|---|---|
| `SystemConfig` struct | 32 B |
| Sector | Bank 2 Sector 7 (0x081E0000–0x081FFFFF) |
| Reserved | 128 KB (from linker via scatter file) |
| Write latency | ~1 s (erase + program, IRQs disabled) |

## Task Parameters

| Task | Stack | Priority | Trigger |
|---|---|---|---|
| `display_lcd` | 16 KB | Normal (3) | Periodic (~500 Hz) |
| `inf` | 8 KB | Normal (3) | Event (frame queue) |
| `mp3` | 4 KB | BelowNormal (1) | Idle (1 Hz) |
| Timer Service | 1 KB | 2 | FreeRTOS internal |
| Idle | 512 B | 0 | FreeRTOS internal |
