# Memory Layout — STM32H743VIT6

## Physical Memory

| Region | Address | Size | Usage |
|---|---|---|---|
| Flash | 0x08000000 | 2MB | Code + AI weights + Config |
| DTCM | 0x20000000 | 128KB | Stack, FreeRTOS heap, critical BSS |
| AXI SRAM | 0x24000000 | 512KB | RGB_DATA (150KB), AI buffers |
| SRAM1+2 | 0x30000000 | 256KB | DCMI DMA buf, LVGL draw buf, .RAM_D2 code |
| SRAM3 | 0x30040000 | 32KB | (unused) |
| SRAM4 | 0x38000000 | 64KB | (unused) |

## Flash Layout (Dual-Bank)

| Bank | Sector | Address | Size | Usage |
|---|---|---|---|---|
| 1 | 0-6 | 0x08000000 | 896KB | Code + RO data |
| 1 | 7 | 0x080E0000 | 128KB | (code/RO) |
| 2 | 0-6 | 0x08100000 | 896KB | Code + AI weights |
| **2** | **7** | **0x081E0000** | **128KB** | **Config storage (32B used)** |

## Scatter File

```
LR_IROM1 0x08000000 0x00200000 {         ; 2MB total
  ER_IROM1 0x08000000 0x001E0000 {       ; reserve Bank2 Sector7
    *.o (RESET, +First)
    .ANY (+RO +XO)
  }
  RW_IRAM1 0x20000000 0x00020000 { .ANY (+RW +ZI) }    ; DTCM
  RW_IRAM2 0x24000000 0x00080000 { .AXI_RAM .AI_RAM }  ; AXI SRAM
  RW_IRAM3 0x30000000 0x00040000 { .D2_RAM .RAM_D2 }   ; SRAM1+2
}
```

## Key Allocations

| Symbol | Section | Size | Notes |
|---|---|---|---|
| `RGB_DATA[320][240]` | .AXI_RAM | 150KB | Camera frame buffer |
| `RGB_DATA_Contiguous[]` | .AXI_RAM | 150KB | LCD DMA buffer |
| `ai_input_data[64*64*3]` | AI_RAM | 48KB | AI input tensor |
| `pool0[]` | default | ~40KB | AI activations |
| `RGB_Line_Buf[2][640]` | .D2_RAM | 2.5KB | DCMI double-buffer |
| `SystemConfig g_cfg` | DTCM | 32B | Config RAM cache |
| `flash_storage_write()` | .RAM_D2 | ~1KB | Flash write (runs from SRAM) |
| FreeRTOS heap | DTCM | 64KB | Task stacks + dynamic |
