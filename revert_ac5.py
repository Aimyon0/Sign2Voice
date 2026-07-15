import os, re
from ruamel.yaml import YAML
yaml = YAML()
yaml.preserve_quotes = True

base = 'e:/base/finaltest'

# 1. Restore Keil startup
os.system(f'copy "{base}/MDK-ARM/startup_stm32h743xx_keil.s.bak" "{base}/MDK-ARM/startup_stm32h743xx.s" /Y')
print('Startup restored')

# 2. EIDE — full revert
with open(f'{base}/MDK-ARM/.eide/eide.yml', 'r', encoding='utf-8') as f:
    config = yaml.load(f)

for tname, tcfg in config['targets'].items():
    tcfg['toolchain'] = 'AC5'
    if 'GCC' in tcfg.get('toolchainConfigMap', {}):
        del tcfg['toolchainConfigMap']['GCC']
    tcfg['toolchainConfigMap']['AC5'] = {
        'archExtensions': '',
        'cpuType': 'Cortex-M7',
        'floatingPointHardware': 'double',
        'options': {
            'version': 4,
            'afterBuildTasks': [],
            'asm-compiler': {},
            'beforeBuildTasks': [],
            'c/cpp-compiler': {
                'CXX_FLAGS': '',
                'C_FLAGS': '',
                'c99-mode': True,
                'one-elf-section-per-function': True,
                'optimization': 'level-0',
                'warnings': 'all-warnings'
            },
            'global': {
                'output-debug-info': 'enable',
                'use-microLIB': False
            },
            'linker': {
                '$outputTaskExcludes': ['.bin'],
                'output-format': 'elf'
            }
        }
    }
    tcfg['scatterFilePath'] = 'finaltest.sct'

# Restore App files
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        for sf in folder.get('folders', []):
            if sf['name'] == 'User':
                for ssf in sf.get('folders', []):
                    if ssf['name'] == 'X-CUBE-AI':
                        for appf in ssf.get('folders', []):
                            if appf['name'] == 'App':
                                appf['files'] = [
                                    {'path': '../X-CUBE-AI/App/gesture_finetuned.c'},
                                    {'path': '../X-CUBE-AI/App/gesture_finetuned_data.c'},
                                    {'path': '../X-CUBE-AI/App/gesture_finetuned_data_params.c'},
                                    {'path': '../X-CUBE-AI/App/app_x-cube-ai.c'},
                                ]
    # FreeRTOS port back
    if folder['name'] == 'Middlewares':
        for mf in folder.get('folders', []):
            if mf['name'] == 'FreeRTOS':
                new_files = []
                for f in mf['files']:
                    if 'portable/GCC/ARM_CM7' in f.get('path', ''):
                        new_files.append({'path': '../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c'})
                    else:
                        new_files.append(f)
                mf['files'] = new_files
    # Lib — add back Keil lib
    if folder['name'] == 'Lib':
        folder['files'] = [{'path': '../Middlewares/ST/AI/Lib/NetworkRuntime1010_CM7_Keil.lib'}]

# Remove GCC runtime from App
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        for sf in folder.get('folders', []):
            if sf['name'] == 'User':
                for ssf in sf.get('folders', []):
                    if ssf['name'] == 'X-CUBE-AI':
                        for appf in ssf.get('folders', []):
                            if appf['name'] == 'App':
                                appf['files'] = [f for f in appf['files'] if 'GCC.a' not in f.get('path','')]

# Fix include paths
for tname, tcfg in config['targets'].items():
    inc = tcfg['cppPreprocessAttrs']['incList']
    # RVDS port back
    old_gcc = '../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1'
    new_rvds = '../Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F'
    if old_gcc in inc:
        inc[inc.index(old_gcc)] = new_rvds

with open(f'{base}/MDK-ARM/.eide/eide.yml', 'w', encoding='utf-8') as f:
    yaml.dump(config, f)
print('EIDE reverted to AC5')

# 3. Restore app_x-cube-ai.c
with open(f'{base}/X-CUBE-AI/App/app_x-cube-ai.c', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()

# Fix includes
c = c.replace('#include "stai.h"\n#include "network.h"\n#include "app_config.h"\n#include "user_init.h"',
              '#include "ai_datatypes_defines.h"\n#include "gesture_finetuned.h"\n#include "gesture_finetuned_data.h"')

# Remove ST AI globals
c = re.sub(r'/\* ST AI network context \*/.*?stai_ptr stai_output\[STAI_NETWORK_OUT_NUM\];\n', '', c, flags=re.DOTALL)
c = re.sub(r'STAI_NETWORK_CONTEXT_DECLARE.*?\n', '', c)
c = re.sub(r'/\* Activations buffer \*/.*?data_activations\[\] = \{ activations_buf \};\n', '', c, flags=re.DOTALL)

# Remove aiInit
c = re.sub(r'/\* ST AI bootstrap \*/.*?^\}', '', c, flags=re.DOTALL | re.MULTILINE)

# Fix MX_X_CUBE_AI_Init
m5 = re.search(r'/\* USER CODE BEGIN 5 \*/.*?/\* USER CODE END 5 \*/', c, re.DOTALL)
if m5:
    c = c.replace(m5.group(0), '    /* USER CODE BEGIN 5 */\n  ai_boostrap(data_activations0);\n    /* USER CODE END 5 */')

# Fix MX_X_CUBE_AI_Process
m6 = re.search(r'/\* USER CODE BEGIN 6 \*/.*?/\* USER CODE END 6 \*/', c, re.DOTALL)
if m6:
    new6 = '''    /* USER CODE BEGIN 6 */
    int res = -1;
    if (gesture_finetuned) {
        int count = 0;
        do {
            res = acquire_and_process_data(data_ins);
            if (res != 0) break;
            for (int i = 0; i < AI_GESTURE_FINETUNED_IN_NUM; i++) {
                ai_input[i].data = (void*)data_ins[i];
            }
            for (int i = 0; i < AI_GESTURE_FINETUNED_OUT_NUM; i++) {
                ai_output[i].data = data_outs[i];
            }
            res = ai_run();
            if (res != 0) break;
            res = post_process(data_outs);
            count++;
            if (count > 1) break;
        } while (res == 0);
        if (res != 0) {
            ai_error err = {AI_ERROR_INVALID_STATE, AI_ERROR_CODE_NETWORK};
            ai_log_err(err, "Process has FAILED");
        }
    } else {
        ai_error err = {AI_ERROR_INVALID_HANDLE, AI_ERROR_CODE_NETWORK};
        ai_log_err(err, "model is NULL");
    }
    /* USER CODE END 6 */'''
    c = c.replace(m6.group(0), new6)

# Restore global variables
old_g = '/* Global variables -----------------------------------------------------------*/'
idx_g = c.find(old_g)
idx_ep = c.find('/* Entry points', idx_g) if idx_g >= 0 else -1
if idx_g >= 0 and idx_ep >= 0:
    c = c[:idx_g] + '''/* Global variables -----------------------------------------------------------*/
const char* class_names[] = {
    "fist", "like", "no_gesture", "ok", "palm"
};

int g_predicted_class = -1;
float g_predicted_prob = 0.0f;

''' + c[idx_ep:]

# Restore USER CODE 2
m2 = re.search(r'/\* USER CODE BEGIN 2 \*/.*?/\* USER CODE END 2 \*/', c, re.DOTALL)
if m2:
    new2 = '''/* USER CODE BEGIN 2 */
#define THRESHOLD 0.1f

int acquire_and_process_data(ai_i8* data[])
{
    data[0] = (ai_i8*)ai_input_data;
    return 0;
}

int post_process(ai_i8* data[])
{
    float* output = (float*)data[0];
    const int class_num = 5;

    int max_index = 0;
    float max_prob = output[0];

    for (int i = 1; i < class_num; ++i) {
        if (output[i] > max_prob) {
            max_prob = output[i];
            max_index = i;
        }
    }

    if (max_prob < 0.0f) max_prob = 0.0f;
    if (max_prob > 1.0f) max_prob = 1.0f;

    g_predicted_prob = max_prob;
    g_predicted_class = max_index;

    return 0;
}
  /* USER CODE END 2 */'''
    c = c.replace(m2.group(0), new2)

with open(f'{base}/X-CUBE-AI/App/app_x-cube-ai.c', 'w', encoding='utf-8') as f:
    f.write(c)
print('app_x-cube-ai.c reverted')

# 4. Restore app_x-cube-ai.h
with open(f'{base}/X-CUBE-AI/App/app_x-cube-ai.h', 'w', encoding='utf-8') as f:
    f.write('''#ifndef __APP_AI_H
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
''')
print('app_x-cube-ai.h reverted')

# 5. Restore main.c
with open(f'{base}/Core/Src/main.c', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()
c = c.replace('/* USER CODE BEGIN Includes */\n#include "system.h"',
              '/* USER CODE BEGIN Includes */\n#include "ai_platform.h"\n#include "gesture_finetuned.h"\n#include "gesture_finetuned_data.h"\n#include "system.h"')
c = c.replace('extern uint8_t  OV_mode;\nextern uint16_t Camera_ID;',
              'ai_handle network_handle = AI_HANDLE_NULL;\nextern uint8_t  OV_mode;\nextern uint16_t Camera_ID;')
with open(f'{base}/Core/Src/main.c', 'w', encoding='utf-8') as f:
    f.write(c)
print('main.c reverted')

# 6. Restore usart.c
with open(f'{base}/Core/Src/usart.c', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()
c = c.replace('''/* USER CODE BEGIN 1 */
#if defined(__CC_ARM)
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

void _exit(int status) { (void)status; while(1); }
void _sys_exit(int x) { x = x; }
int _getpid(void) { return 1; }
int _kill(int pid, int sig) { (void)pid; (void)sig; return -1; }
int _write(int fd, const char *buf, int len) { (void)fd; (void)buf; return len; }
int _read(int fd, char *buf, int len) { (void)fd; (void)buf; return len; }
int _close(int fd) { (void)fd; return -1; }
int _lseek(int fd, int ptr, int dir) { (void)fd; (void)ptr; (void)dir; return 0; }
int _isatty(int fd) { (void)fd; return 1; }
int _fstat(int fd, void *buf) { (void)fd; (void)buf; return 0; }
void *_sbrk(int incr) { extern char _ebss; static char *heap_end; char *prev; if(!heap_end) heap_end = &_ebss; prev = heap_end; heap_end += incr; return prev; }

int fputc(int ch, FILE *f)''',
              '''/* USER CODE BEGIN 1 */
#if 1
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}

int fputc(int ch, FILE *f)''')
with open(f'{base}/Core/Src/usart.c', 'w', encoding='utf-8') as f:
    f.write(c)
print('usart.c reverted')

# 7. Also fix the standard AC5 issues
with open(f'{base}/Core/Inc/FreeRTOSConfig.h', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()
c = c.replace('#define configENABLE_FPU                         0', '#define configENABLE_FPU                         1')
with open(f'{base}/Core/Inc/FreeRTOSConfig.h', 'w', encoding='utf-8') as f:
    f.write(c)

with open(f'{base}/Core/Src/main.c', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()
c = c.replace('  MX_SPI2_Init();\n  MX_X_CUBE_AI_Init();\n  /* USER CODE BEGIN 2 */',
              '  MX_SPI2_Init();\n  /* USER CODE BEGIN 2 */')
c = c.replace('  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)',
              '  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)')
c = c.replace('  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\n  {\n    Error_Handler();\n  }\n\n  /** Initializes the CPU',
              '  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)\n  {\n    Error_Handler();\n  }\n  if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET) Error_Handler();\n  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET) Error_Handler();\n\n  /** Initializes the CPU')
with open(f'{base}/Core/Src/main.c', 'w', encoding='utf-8') as f:
    f.write(c)

# Check freertos.c has StackOverflowHook
with open(f'{base}/Core/Src/freertos.c', 'r', encoding='utf-8', errors='replace') as f:
    c = f.read()
if 'StackOverflowHook' not in c:
    c = c.replace('/* USER CODE END GET_IDLE_TASK_MEMORY */\n\n/**',
                  '/* USER CODE END GET_IDLE_TASK_MEMORY */\n\nvoid vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)\n{\n    taskDISABLE_INTERRUPTS();\n    for(;;);\n}\n\n/**')
    with open(f'{base}/Core/Src/freertos.c', 'w', encoding='utf-8') as f:
        f.write(c)
    print('StackOverflowHook added')

# Remove GCC .o files
os.system(f'rm -rf "{base}/X-CUBE-AI/App/build_rt" 2>nul')
os.system(f'rm -rf "{base}/X-CUBE-AI/App/stai.h" 2>nul')

print('\n=== AC5 revert complete ===')
print('Compile now.')
