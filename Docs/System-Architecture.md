# System Architecture

> Insert diagram: Camera → DCMI → Frame Buffer → Preprocessing → CNN → Result Queue → Display + Audio

## Data Flow

1. **OV2640 Camera** captures 320×240 RGB565 frames at 30fps
2. **DCMI + DMA** transfers pixel data directly to `RGB_DATA[240][320]` in AXI SRAM
3. **D-Cache Invalidate** ensures CPU sees the latest frame
4. **Bilinear Resize** converts 320×240 RGB565 → 64×64×3 RGB888 Float (~78 ms)
5. **CNN Inference** runs INT8 quantized model via X-CUBE-AI (~135 ms)
6. **Result Queue** (osMailQ) delivers `{class, probability, latency}` to display task
7. **Sliding Window Filter** confirms gesture after N consecutive frames
8. **Display + Audio** shows result on ILI9341 LCD and plays MP3 via YX5200

## Key Design Decisions

- **Async IPC**: Camera/UI and AI inference run in separate FreeRTOS tasks, decoupled by dual message queues
- **D-Cache management**: `SCB_InvalidateDCache_by_Addr()` before reading DMA-written buffers
- **Sliding window**: 3/5/7-frame voting eliminates single-frame jitter without Kalman complexity
