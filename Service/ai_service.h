/**
 * @file    ai_service.h
 * @brief   Model-agnostic AI inference service.
 *          Wraps X-CUBE-AI generated code behind a clean, swappable API.
 *
 * Usage:
 *   1. call ai_service_init() once before scheduler starts.
 *   2. call ai_service_run() from the inference task each time a
 *      preprocessed input buffer is ready.
 *   3. read results via ai_service_get_class() / ai_service_get_prob().
 */

#ifndef AI_SERVICE_H
#define AI_SERVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Init / Run ---- */

/** One-time init. Must be called before scheduler starts. */
int  ai_service_init(void);

/** Run one inference on the preloaded input buffer.
 *  Updates internal class / probability / timing state. */
int  ai_service_run(void);

/* ---- Results (valid after ai_service_run returns 0) ---- */

int         ai_service_get_class(void);
float       ai_service_get_prob(void);
uint32_t    ai_service_get_inf_ms(void);

/* ---- Metadata ---- */

/** Number of output classes. */
int         ai_service_get_class_count(void);

/** Human-readable name for class `cls` (0-based). */
const char* ai_service_get_class_name(int cls);

/* ---- Input buffer access (for the camera→AI data path) ---- */

/** Pointer to the model input buffer (64×64×3 float [0,1] RGB).
 *  The camera service writes here, then ai_service_run() consumes it. */
float* ai_service_get_input_buf(void);

#ifdef __cplusplus
}
#endif

#endif /* AI_SERVICE_H */
