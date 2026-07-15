# AI Deployment Pipeline

```
Keras Model (.h5)
    │  TensorFlow
    ▼
TFLite Model (.tflite) ──── FP32 → INT8 quantization
    │  STM32Cube.AI (X-CUBE-AI)
    ▼
Optimized C Code ─────────── gesture.c / gesture_data.c
    │  ARM Compiler 5 (AC5)
    ▼
Flash (2 MB) ─────────────── Weights stored in ROM
    │  Runtime
    ▼
X-CUBE-AI Runtime API ───── ai_gesture_create_and_init()
    │                       ai_gesture_run()
    ▼
STM32H743 @400 MHz ──────── ~135 ms per inference
```

## Model Details

| Property | Value |
|----------|-------|
| Input | 64×64×3 RGB888 Float |
| Output | 5-class softmax (Fist/Like/None/OK/Palm) |
| Quantization | INT8 |
| Framework | TensorFlow / Keras |
| Test Accuracy | 87.7% |
| Weights Size | ~110 KB (Flash) |
| Activations | ~85 KB (RAM) |

## Runtime API (`AI/app_x-cube-ai.c`)

```c
// Init
ai_boostrap(data_activations0);

// Preprocess
RGB565_To_64x64_RGB888_Float(RGB_DATA, 320, 240, ai_input_data);

// Run
acquire_and_process_data(data_ins);
ai_gesture_run(model, ai_input, ai_output);
post_process(data_outs);  // argmax → g_predicted_class, g_predicted_prob
```
