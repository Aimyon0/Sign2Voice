import re

with open('e:/base/finaltest/X-CUBE-AI/App/app_x-cube-ai.c', 'r', encoding='utf-8', errors='replace') as f:
    content = f.read()

# 1. Replace old AI includes
content = content.replace(
    '#include "ai_datatypes_defines.h"\n#include "gesture_finetuned.h"\n#include "gesture_finetuned_data.h"',
    '#include "stai.h"\n#include "network.h"\n#include "app_config.h"'
)

# 2. Clear USER CODE BEGIN 2
m = re.search(r'/\* USER CODE BEGIN 2 \*/.*?/\* USER CODE END 2 \*/', content, re.DOTALL)
if m:
    content = content.replace(m.group(0), '/* USER CODE BEGIN 2 */\n/* USER CODE END 2 */')
    print('USER CODE 2 cleared')

# 3. Fix Global variables
old_glob = '/* Global variables -----------------------------------------------------------*/'
idx = content.find(old_glob)
if idx >= 0:
    end_idx = content.find('/* Entry points', idx)
    if end_idx >= 0:
        new_glob = '''/* Global variables -----------------------------------------------------------*/
const char* class_names[] = {
    "fist", "like", "no_gesture", "ok", "palm"
};

int g_predicted_class = -1;
float g_predicted_prob = 0.0f;

/* ST AI network context */
STAI_NETWORK_CONTEXT_DECLARE(network_context, STAI_NETWORK_CONTEXT_SIZE)

stai_size in_length, out_length;
stai_ptr stai_input[STAI_NETWORK_IN_NUM];
stai_ptr stai_output[STAI_NETWORK_OUT_NUM];

'''
        content = content[:idx] + new_glob + content[end_idx:]
        print('Globals updated')

# 4. Fix MX_X_CUBE_AI_Init (USER CODE 5)
m5 = re.search(r'/\* USER CODE BEGIN 5 \*/.*?/\* USER CODE END 5 \*/', content, re.DOTALL)
if m5:
    new5 = '/* USER CODE BEGIN 5 */\n  aiInit();\n    /* USER CODE END 5 */'
    content = content.replace(m5.group(0), new5)
    print('Init updated')

# 5. Fix MX_X_CUBE_AI_Process (USER CODE 6)
m6 = re.search(r'/\* USER CODE BEGIN 6 \*/.*?/\* USER CODE END 6 \*/', content, re.DOTALL)
if m6:
    new6 = '''/* USER CODE BEGIN 6 */
    extern stai_ptr stai_input[];
    extern stai_ptr stai_output[];
    stai_return_code ret;

    /* copy preprocessed data to model input */
    memcpy(stai_input[0], ai_input_data, STAI_NETWORK_IN_1_SIZE_BYTES);

    /* run inference */
    ret = stai_network_run(network_context, STAI_MODE_SYNC);
    if (ret != STAI_SUCCESS) return;

    /* post-process: argmax on float32 output */
    {
        float *out = (float*)stai_output[0];
        const int class_num = 5;
        int max_index = 0;
        float max_prob = out[0];
        for (int i = 1; i < class_num; i++) {
            if (out[i] > max_prob) { max_prob = out[i]; max_index = i; }
        }
        if (max_prob < 0.0f) max_prob = 0.0f;
        if (max_prob > 1.0f) max_prob = 1.0f;
        g_predicted_prob = max_prob;
        g_predicted_class = max_index;
    }
    /* USER CODE END 6 */'''
    content = content.replace(m6.group(0), new6)
    print('Process updated')

with open('e:/base/finaltest/X-CUBE-AI/App/app_x-cube-ai.c', 'w', encoding='utf-8') as f:
    f.write(content)
print('Saved')
