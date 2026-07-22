# Debug Guide

## Log System

Compile-time controlled log macros in `Debug/log.h`:

```c
#define LOG_LEVEL_NONE  0   // All logs removed (production)
#define LOG_LEVEL_ERROR 1   // Errors only
#define LOG_LEVEL_WARN  2   // Errors + Warnings
#define LOG_LEVEL_INFO  3   // All (development default)
```

### Usage

```c
LOG_ERROR("flash_write: erase failed");
LOG_WARN("config_load: invalid data, using defaults");
LOG_INFO("gesture_app: detected %s (%.0f%%)", name, prob * 100);
```

### Compile-time Control

```c
// In Debug/log.h:
#define LOG_LEVEL LOG_LEVEL_NONE   // Production: zero overhead

// Or via -D flag in EIDE compiler settings:
// C_FLAGS: -DLOG_LEVEL=3
```

### UART Output

Logs output via `printf()` → USART1 PA9 TX. Baud rate: 115200 at boot, 9600 after MP3_Init.
Connect USB-TTL at 115200 for boot messages, 9600 for runtime messages.
**Note**: USART1 shared with MP3 module. Enable LOG_LEVEL only for debugging.

### Fault Handlers

HardFault/MemManage/BusFault/UsageFault handlers in `stm32h7xx_it.c` dump registers to UART on crash:
- Stacked: R0-R3, R12, LR, PC, xPSR
- CFSR, HFSR, BFAR, MMFAR

### FreeRTOS Debugging

- `configCHECK_FOR_STACK_OVERFLOW = 2` — checks both overflow directions
- `vApplicationStackOverflowHook` — disables IRQs on overflow
- `configASSERT` — hangs with IRQs disabled on assertion failure
- Heap: 64KB (`configTOTAL_HEAP_SIZE`), monitored via `xPortGetFreeHeapSize()`
