# CHANGELOG v2.1 — Reliability & Maintainability

## New Modules

| Module | Path | Description |
|---|---|---|
| ErrorCode | Common/error_code.h | Unified error codes for all Service modules |
| Log | Debug/log.h, log.c | Compile-time controlled LOG_INFO/WARN/ERROR macros |
| Config | Config/config.h, config.c | SystemConfig struct with get/set/load/save/reset |
| Flash Storage | Storage/flash_storage.c | Bank 2 Sector 7 read/write (SRAM-resident write) |
| Docs | Docs/*.md | Architecture, Configuration, ErrorHandling, DebugGuide, MemoryLayout |

## Modified Files

| File | Changes |
|---|---|
| Service/ai_service.h/c | ErrorCode returns, start/stop/deinit, init guard |
| Service/camera_service.h/c | ErrorCode returns, stop/deinit, NULL pointer check |
| Service/audio_service.h/c | ErrorCode returns, start/stop/deinit, 2s cooldown + dedup |
| Service/ui_service.h/c | ErrorCode returns, start/stop/deinit |
| App/gesture_app.h/c | config_get_* API, 2s cooldown timer, bounds check |
| Core/Src/menu.c | config_* API replaces g_vol/g_thr/g_bri statics, config_save on exit, reads actual values for settings page labels |
| Core/Src/freertos.c | audio_init before menu_init, config.h include |
| Core/Src/main.c | config_init, log_init at boot |
| Core/Inc/lv_conf.h | Fixed duplicate LV_USE_BUTTON/KEYBOARD/LOTTIE |
| MDK-ARM/finaltest.sct | ER_IROM1 0x001E0000 reserves Bank2 Sector7 |
| MDK-ARM/.eide/eide.yml | Added Config/Debug/Storage paths + source files |

## Bug Fixes

- **Volume desync**: menu.c and audio_service.c now share config_get_volume() as single source of truth
- **Settings display**: f_vol/f_thr/f_bright now read actual config values instead of hardcoded defaults
- **Gesture loop**: Added 2-second cooldown + dedup to prevent rapid MP3 re-triggering
- **Flash Bank mismatch**: STM32H743 is DUAL-BANK; flash_storage_write used wrong bank (FLASH_BANK_1 → FLASH_BANK_2)
- **Scatter file overlap**: ER_IROM1 reduced to reserve Sector 7 from linker
- **lv_conf.h**: Removed duplicate LV_USE_BUTTON/KEYBOARD/LOTTIE macro definitions

## Flash Persistence Debugging

Flash storage was the most complex v2.1 feature. See `Docs/Configuration.md` for full debugging history. Key fixes:
1. Bank mismatch (BANK_2 for address 0x081E0000 in dual-bank mode)
2. Scatter file reservation (0x001E0000 not 0x00200000)
3. SRAM-resident write function (.RAM_D2 section)
4. D-Cache maintenance (SCB_CleanDCache before flash write)
5. Programming delay for 400MHz HCLK (__HAL_FLASH_SET_PROGRAM_DELAY)

## Deferred to v2.2

- IWDG Watchdog (needs HAL IWDG driver or verified LSI init)
- UART separation for debug logs (USART2 PA2/PA3)
- Runtime timeout recovery for camera/AI/MP3
