import os, shutil, fnmatch

SRC = r'e:\base\finaltest'
DST = r'e:\base\finaltest_submit'

if os.path.exists(DST):
    shutil.rmtree(DST)
os.makedirs(DST)

KEEP = [
    'Core/Inc/*.h',
    'Core/Src/*.c',
    'X-CUBE-AI/App/gesture_finetuned*',
    'X-CUBE-AI/App/app_x-cube-ai.*',
    'Middlewares/Third_Party/FreeRTOS/**/*',
    'Middlewares/Third_Party/LVGL/**/*',
    'Middlewares/ST/AI/**/*',
    'Drivers/STM32H7xx_HAL_Driver/Src/*.c',
    'Drivers/STM32H7xx_HAL_Driver/Inc/**/*.h',
    'Drivers/CMSIS/Device/ST/STM32H7xx/**/*',
    'Drivers/CMSIS/Include/*.h',
    'MDK-ARM/HARDWARE/**/*',
    'MDK-ARM/*.sct',
    'MDK-ARM/*.uvprojx',
    'MDK-ARM/startup_stm32h743xx.s',
    'MDK-ARM/system.c',
    'MDK-ARM/.eide/**/*',
    'docs/*.html',
    'docs/*.md',
    'model/*.tflite',
    'model/*.h5',
    'check.bat',
]

SKIP_DIRS = {
    'MP3-TF-16P', 'MP3-TF-16P模块使用说明书',
    'MDK-ARM/finaltest', 'MDK-ARM/build', 'MDK-ARM/.cmsis',
    'MDK-ARM/DebugConfig', 'MDK-ARM/RTE', 'MDK-ARM/Drivers',
    'MDK-ARM/1',
    'Drivers/BSP', 'Drivers/CMSIS/DSP', 'Drivers/CMSIS/Core_A',
    'Drivers/CMSIS/Core', 'Drivers/CMSIS/Documentation',
    'Drivers/CMSIS/RTOS2',
    '.ai', '.git', '.vscode', 'model/data', 'model/model',
}

def matches(path, pattern):
    return fnmatch.fnmatch(path.replace('\\', '/'), pattern)

total = 0
count = 0

for root, dirs, files in os.walk(SRC):
    rel_dir = os.path.relpath(root, SRC).replace('\\', '/')
    if rel_dir == '.':
        rel_dir = ''

    # Skip dirs
    to_skip = set()
    for d in dirs:
        rd = (rel_dir + '/' + d).lstrip('/')
        for sd in SKIP_DIRS:
            if rd == sd or rd.startswith(sd + '/'):
                to_skip.add(d)
                break
    dirs[:] = [d for d in dirs if d not in to_skip]

    for f in files:
        rp = (rel_dir + '/' + f).lstrip('/')
        ok = any(matches(rp, p) for p in KEEP)
        if not ok:
            continue

        src_f = os.path.join(root, f)
        dst_d = os.path.join(DST, rel_dir)
        os.makedirs(dst_d, exist_ok=True)

        try:
            shutil.copy2(src_f, os.path.join(dst_d, f))
            total += os.path.getsize(src_f)
            count += 1
        except:
            pass

# Also copy .eide level files
for extra in ['env.ini', 'files.options.yml']:
    sf = os.path.join(SRC, 'MDK-ARM', '.eide', extra)
    df = os.path.join(DST, 'MDK-ARM', '.eide', extra)
    if os.path.exists(sf):
        os.makedirs(os.path.dirname(df), exist_ok=True)
        shutil.copy2(sf, df)

print(f'Files: {count}, Size: {total/1024/1024:.1f} MB')
print(f'Output: {DST}')
