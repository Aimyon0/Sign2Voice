# Error Handling

## Error Codes

```c
typedef enum {
    ERR_OK              =  0,
    ERR_CAMERA          = -1,
    ERR_AI              = -2,
    ERR_AUDIO           = -3,
    ERR_LCD             = -4,
    ERR_FLASH           = -5,
    ERR_TIMEOUT         = -6,
    ERR_INVALID_PARAM   = -7,
    ERR_NULL_POINTER    = -8,
    ERR_BUSY            = -9,
} ErrorCode;
```

All Service modules return `ErrorCode` instead of raw `int`. Each code maps to a specific subsystem, enabling targeted error recovery.

## Module Error Handling

### Camera Service
- `camera_service_get_rgb888()`: returns `ERR_NULL_POINTER` if buffer is NULL

### AI Service
- `ai_service_run()`: returns `ERR_AI` if called before `ai_service_init()`

### Audio Service
- `audio_service_play_class()`: returns -1 for out-of-range class (0-4), silently skips `no_gesture` (track=-1)
- 2-second cooldown in `gesture_app_on_result()` prevents rapid re-triggering

### Flash Storage
- `flash_storage_read/write()`: returns `ERR_INVALID_PARAM` for NULL or oversized buffer, `ERR_FLASH` on hardware failure
- IRQs disabled during write; function runs from SRAM to survive flash stall

## Fatal Faults

| Fault | Handler | Behavior |
|---|---|---|
| HardFault | stm32h7xx_it.c | Dump stacked registers to UART, infinite loop |
| MemManage | stm32h7xx_it.c | Dump CFSR + MMFAR |
| BusFault | stm32h7xx_it.c | Dump CFSR + BFAR |
| UsageFault | stm32h7xx_it.c | Dump CFSR |
| Stack Overflow | freertos.c hook | Disable IRQs, infinite loop |

## Gesture Pipeline Resilience

- **Sliding window**: Requires N consecutive same-class frames above threshold before confirming
- **Cooldown**: 2-second lockout after each confirmed gesture prevents rapid oscillation
- **Menu reset**: Sliding window state cleared when entering recognition mode from menu
