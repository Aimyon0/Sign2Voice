# Sign2Voice — Troubleshooting Guide

Hard-won debugging experience across v1.0 through v2.2.

---

## 1. Flash Persistence HardFault

**Symptom**: System HardFaults immediately after `config_save()` is called from menu.

**Root Cause**: Three stacked bugs.

#### Bug 1: Bank Mismatch (v2.1)

STM32H743VIT6 is **dual-bank** (`DUAL_BANK` defined in `stm32h743xx.h`).
Address 0x081E0000 = **Bank 2 Sector 7**. But the code was erasing **Bank 1 Sector 7**.

```
Flash layout (dual-bank):
  Bank 1: 0x08000000–0x080FFFFF  (Sectors 0–7)
  Bank 2: 0x08100000–0x081FFFFF  (Sectors 0–7)

We wrote: erase.Banks = FLASH_BANK_1, erase.Sector = FLASH_SECTOR_7
  → Erased 0x080E0000 (Bank 1 Sector 7) — where code lives!
  → Program 0x081E0000 (Bank 2 Sector 7) — different bank!

Fix: erase.Banks = FLASH_BANK_2
```

#### Bug 2: Scatter File Overlap (v2.1)

Original scatter file:
```
ER_IROM1 0x08000000 0x00200000  { .ANY (+RO +XO) }
```
Linker could place code anywhere in 2MB, including 0x081E0000.
Erasing Sector 7 destroyed arbitrary code.

Fix: `ER_IROM1 0x08000000 0x001E0000` — reserves last 128KB.

#### Bug 3: SRAM Execution (v2.1)

STM32H7 stalls instruction fetch from Flash during erase/program.
The write function MUST run from SRAM:
```c
__attribute__((section(".RAM_D2")))
ErrorCode flash_storage_write(...) { ... }
```
Without this, CPU hangs on the instruction after `SET_BIT(FLASH->CR, FLASH_CR_START)`.

#### Additional Fixes

- **Pointer arithmetic**: `(uint32_t)(buf + i*32)` → `(uint32_t)&buf[i*32]` for unambiguous address extraction
- **D-Cache**: `SCB_CleanDCache_by_Addr` before write ensures data flushed to physical SRAM
- **Program delay**: `__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_BANK_2)` for reliable operation at 400MHz

---

## 2. Boot HardFault (v1.0)

**Symptom**: System HardFaults immediately after power-on.

**Causes (3 independent bugs)**:

| Bug | Symptom | Fix |
|---|---|---|
| `configENABLE_FPU = 0` | Cortex-M7 FPU disabled; float instructions HardFault | Set to `1` in FreeRTOSConfig.h |
| `FLASH_LATENCY_2` at 400MHz | Wait states insufficient for HCLK=200MHz | Changed to `FLASH_LATENCY_4` |
| `configAPPLICATION_ALLOCATED_HEAP = 1` | Heap pointer conflict with FreeRTOS static allocation | Removed the define |

**Diagnostic method**: Read stacked registers from `HardFault_Handler`:
- PC tells you which instruction faulted
- CFSR bits identify the fault type (IACCVIOL, DACCVIOL, UNALIGNED, etc.)

---

## 3. MP3 Playback Issues

#### Intermittent Trigger (v1.0 → v2.1)

**Symptom**: MP3 sometimes plays, sometimes silent after gesture detected.

**Root Cause 1 — UART conflict**: Both `printf` (USART1 via `huart1`) and MP3 module (USART1 via `MP3_UART_Handler`) shared PA9/PA10. `LOG_INFO` in the gesture path called `printf` right after `MP3_PlayIndex` sent a UART frame. Two handler structs on the same hardware caused TX collisions.

Fix: Set `LOG_LEVEL LOG_LEVEL_NONE` in production builds.

**Root Cause 2 — Volume desync**: `menu.c` called `MP3_SetVolume()` directly, bypassing `audio_service`. The audio service maintained its own `g_volume` that was never synced.

Fix: All volume changes go through `audio_service_set_volume()`.

#### Continuous Re-trigger (v2.1)

**Symptom**: MP3 kept playing same track or cycling through all 4 tracks.

**Fixes**:
1. **Dedup latch** in `audio_service_play_class`: `track != g_last_play` prevents replaying same gesture
2. **2-second cooldown** in `gesture_app_on_result`: `g_cooldown = HAL_GetTick() + 2000` blocks ALL audio for 2s after each confirmation

#### Init Order (v2.1)

**Symptom**: First volume command silently dropped.

**Root Cause**: `menu_init()` called `audio_service_set_volume()` BEFORE `audio_service_init()` had initialized `MP3_UART_Handler`. The UART handler was zeroed (BSS), so `HAL_UART_Transmit` returned `HAL_BUSY` silently.

Fix: Move `audio_service_init()` before `menu_init()` in the display task.

---

## 4. Black Screen After Flash (v2.1)

**Symptom**: Compile → Flash → system boots to black screen. Pressing RESET briefly shows previous LVGL frame, then black.

**Root Cause**: `watchdog_init()` called in `main()` before scheduler started, trying to access IWDG registers while LSI oscillator was not yet stable.

Fix: Move `watchdog_init()` to display task (after scheduler starts, ~100ms into boot).

**Other possible causes investigated**:
- `fputc` with `HAL_MAX_DELAY` on USART2 — if TX pin floating, blocks forever (mitigated by using USART1 with timeout)
- Scatter file change from `0x00200000` to `0x001E0000` — reverted until verified

---

## 5. Config Data Corruption (v2.1)

**Symptom**: Wrote `volume=10, threshold=70, window=5` but read back `v=0, t=134, w=1`.

**Root Cause**: `HAL_FLASH_Program` third parameter `DataAddress` is a RAM pointer, not a value. The original code passed a pointer-arithmetic expression that the AC5 compiler optimized ambiguously:
```c
HAL_FLASH_Program(..., (uint32_t)(buf + i * 32));  // wrong
```
Fix:
```c
uint32_t src = (uint32_t)&buf[i * 32];
HAL_FLASH_Program(..., src);  // correct
```

Also: the source buffer must be 32-byte aligned and cleaned from D-Cache before the flash write starts.

---

## 6. Settings Display Bug (v2.1)

**Symptom**: After reboot, settings always displayed "20 / 30" even though actual volume was 8. Pressing KEY1 made it jump from 20 to 9.

**Root Cause**: `f_vol()`, `f_thr()`, `f_bright()` used hardcoded default labels ("20 / 30", "70 %", "5 frames"). The actual config value was loaded correctly into RAM, but the LVGL label was never updated from config.

Fix: Read `config_get_volume()`, `config_get_threshold()`, `config_get_window_size()` at page creation time.

---

## 7. CMSIS-NN Migration (v2.2)

**Steps**:
1. Regenerate model with `stedgeai generate --optimization time` (CubeMX: Optimization → Speed)
2. Copy new model files: `gesture.c/h`, `gesture_config.h`, `gesture_data.c/h`, `gesture_data_params.c/h`
3. Copy new runtime library: `NetworkRuntime1020_CM7_Keil.lib`
4. Update `eide.yml`: `1010` → `1020` in Lib path
5. No CMSIS-NN source files needed (kernels embedded in `.lib`)

**Result**: 135 ms → 123 ms (-9%). Gains limited by small 3×3 convolutions.
