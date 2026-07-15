from ruamel.yaml import YAML
yaml = YAML()
yaml.preserve_quotes = True

with open('e:/base/finaltest/MDK-ARM/.eide/eide.yml', 'r', encoding='utf-8') as f:
    config = yaml.load(f)

# 1. Switch toolchain AC5 -> GCC
for tname, tcfg in config['targets'].items():
    tcfg['toolchain'] = 'GCC'
    # Remove AC5 toolchainConfigMap
    if 'toolchainConfigMap' in tcfg:
        del tcfg['toolchainConfigMap']
    print(f'Toolchain: -> GCC for {tname}')

# 2. Update X-CUBE-AI/App files
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        for sf in folder.get('folders', []):
            if sf['name'] == 'User':
                for ssf in sf.get('folders', []):
                    if ssf['name'] == 'X-CUBE-AI':
                        for appf in ssf.get('folders', []):
                            if appf['name'] == 'App':
                                # New file list for ST AI model
                                appf['files'] = [
                                    {'path': '../X-CUBE-AI/App/network.c'},
                                    {'path': '../X-CUBE-AI/App/network_data.c'},
                                    {'path': '../X-CUBE-AI/App/network_weights.c'},
                                    {'path': '../X-CUBE-AI/App/user_init.c'},
                                    {'path': '../X-CUBE-AI/App/app_x-cube-ai.c'},
                                ]
                                print('App files updated')

# 3. Update FreeRTOS port: RVDS -> GCC
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Middlewares':
        for mf in folder.get('folders', []):
            if mf['name'] == 'FreeRTOS':
                new_files = []
                for f in mf['files']:
                    if 'portable/RVDS/ARM_CM4F/port.c' in f['path']:
                        new_files.append({'path': '../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c'})
                        print('FreeRTOS port: RVDS/ARM_CM4F -> GCC/ARM_CM7')
                    else:
                        new_files.append(f)
                mf['files'] = new_files

# 4. Add runtime library
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        found_lib = False
        for sf in folder.get('folders', []):
            if sf['name'] == 'User':
                for ssf in sf.get('folders', []):
                    if ssf['name'] == 'X-CUBE-AI':
                        for appf in ssf.get('folders', []):
                            if appf['name'] == 'App':
                                # Check if lib already exists
                                paths = {f.get('path','') for f in appf['files']}
                                if '../Middlewares/ST/AI/Lib/NetworkRuntime1201_CM7_GCC.a' not in paths:
                                    appf['files'].append({'path': '../Middlewares/ST/AI/Lib/NetworkRuntime1201_CM7_GCC.a'})
                                    print('Added GCC runtime lib')

# 5. Update include paths
for tname, tcfg in config['targets'].items():
    inc_list = tcfg['cppPreprocessAttrs']['incList']

    # Remove old paths
    remove = [
        '.cmsis/include',
        'RTE/_finaltest',
        '../Middlewares/ST/AI/Inc',
        '../Middlewares/tensorflow',
        '../Middlewares/tensorflow/third_party/flatbuffers/include',
        '../Middlewares/tensorflow/third_party/cmsis/CMSIS/Core/Include',
        '../Middlewares/tensorflow/third_party/gemmlowp',
        '../Middlewares/tensorflow/third_party/cmsis/CMSIS/Core',
    ]
    for p in remove:
        if p in inc_list:
            inc_list.remove(p)

    # Add ST AI headers include
    add = [
        '../X-CUBE-AI/App',  # ST AI headers are here now
        '.cmsis/include',
    ]
    for p in add:
        if p not in inc_list:
            inc_list.append(p)

    print('Include paths updated')

# 6. Remove scatterFilePath, add linker script path
for tname, tcfg in config['targets'].items():
    tcfg['scatterFilePath'] = 'STM32H743VITx_FLASH.ld'
    print(f'Linker script: -> STM32H743VITx_FLASH.ld')

with open('e:/base/finaltest/MDK-ARM/.eide/eide.yml', 'w', encoding='utf-8') as f:
    yaml.dump(config, f)
print('EIDE saved')
