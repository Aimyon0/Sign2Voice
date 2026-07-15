#ifndef __APP_AI_H
#define __APP_AI_H
#ifdef __cplusplus
extern "C" {
#endif
#include "ai_platform.h"
void MX_X_CUBE_AI_Init(void);
void MX_X_CUBE_AI_Process(void);
/* USER CODE BEGIN includes */
extern ai_buffer* ai_input;
extern ai_buffer* ai_output;
extern float ai_input_data[64*64*3];
extern int g_predicted_class;
extern float g_predicted_prob;
extern const char* class_names[];
/* USER CODE END includes */
#ifdef __cplusplus
}
#endif
#endif
