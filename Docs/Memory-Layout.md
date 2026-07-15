# Memory Layout

STM32H743VIT6 has 1 MB of SRAM across 4 independent memory domains.

## Scatter-Loading Configuration (`finaltest.sct`)

| Region | Physical Address | Size | Contents |
|--------|-----------------|------|----------|
| DTCM | 0x2000 0000 | 128 KB | FreeRTOS kernel objects, task stacks, hot variables |
| AXI SRAM | 0x2400 0000 | 512 KB | RGB_DATA (150 KB), AI activations, FreeRTOS heap, LVGL pool |
| SRAM1+2 | 0x3000 0000 | 256 KB | DMA line buffers (`.D2_RAM`), LVGL display buffers (`.RAM_D2`) |
| Flash | 0x0800 0000 | 2 MB | Application code, AI model weights |

## Large Arrays

| Array | Size | Section | Phys. Memory |
|-------|------|---------|-------------|
| `RGB_DATA[320][240]` | 150 KB | `.AXI_RAM` | AXI SRAM |
| `ai_input_data[64×64×3]` | 48 KB | `AI_RAM` | AXI SRAM |
| `RGB_Line_Buf[2][640]` | 5 KB | `.D2_RAM` | SRAM1+2 |
| `disp_buf1[14400]` | 28 KB | `.RAM_D2` | SRAM1+2 |
| `lcd_tx_buf[24000]` | 48 KB | `.RAM_D2` | SRAM1+2 |
| AI activations | ~85 KB | Default | AXI SRAM |
| FreeRTOS heap | 64 KB | Default | AXI SRAM |

## CubeMX Restore Checklist

After CubeMX regeneration, these MUST be manually restored:
1. Scatter file (`finaltest.sct`): add `RW_IRAM3` + `.AXI_RAM` / `.D2_RAM` / `.RAM_D2` sections
2. `FreeRTOSConfig.h`: `configENABLE_FPU = 1` (CRITICAL for FPU context save)
3. System clock: PLLM=2, PLLN=64, PLLP=2 (for 25 MHz HSE → 400 MHz)
4. `FLASH_LATENCY_4` + `VOLTAGE_SCALE0`
