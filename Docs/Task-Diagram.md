# FreeRTOS Task Diagram

```
                    ┌────────────────────┐
                    │   display_lcd      │
                    │   Normal · 4096    │
                    │                    │
                    │  • Camera refresh  │
                    │  • LVGL render     │
                    │  • Key scan        │
                    │  • Sliding window  │
                    │  • MP3 playback    │
                    └──────┬─────────────┘
                           │ osMessagePut (frameQueue)
                    ┌──────▼─────────────┐
                    │   info             │
                    │   Normal · 2048    │
                    │                    │
                    │  • Preprocessing   │
                    │  • AI inference    │
                    └──────┬─────────────┘
                           │ osMailPut (resultQueue)
                    ┌──────▼─────────────┐
                    │   display_lcd      │ ← back to display
                    └────────────────────┘

                    ┌────────────────────┐
                    │   out              │
                    │   Low · 1024       │
                    │   MP3 background   │
                    │   (idle)           │
                    └────────────────────┘
```

## IPC Design

- **Frame Queue** (`osMessageQ`, depth 4): display → info trigger. Non-blocking send; queue full = drop old frame.
- **Result Queue** (`osMailQ`, depth 2): info → display result. Non-blocking poll; no result = skip.

## Why This Design

display_lcd runs at 500 Hz (osDelay 2 ms). It never waits for AI inference.
info blocks on frame queue (portMAX_DELAY). It only consumes CPU when a new frame arrives.
Both tasks share Normal priority; the scheduler time-slices them when info is active.
