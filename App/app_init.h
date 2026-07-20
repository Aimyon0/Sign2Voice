/**
 * @file    app_init.h
 * @brief   Unified application initialisation.
 *          Calls all service-layer inits in the correct order.
 */

#ifndef APP_INIT_H
#define APP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/** Call once from main(), after HAL and peripheral inits,
 *  before MX_FREERTOS_Init() / osKernelStart(). */
int app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_INIT_H */
