from ruamel.yaml import YAML
yaml = YAML()
yaml.preserve_quotes = True

with open('e:/base/finaltest/MDK-ARM/.eide/eide.yml', 'r', encoding='utf-8') as f:
    config = yaml.load(f)

# 1. Fix model file names
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
                                print('Model files updated')

# 2. Add missing Core files
core_add = ['lv_port_disp.c','lv_port_indev.c','lv_port_tick.c','ui_overlay.c',
            'camera_band_refresh.c','menu.c','gpio.c','dma.c','spi.c','usart.c']
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        for sf in folder.get('folders', []):
            if sf['name'] == 'User':
                for ssf in sf.get('folders', []):
                    if ssf['name'] == 'Core':
                        existing = {f['path'].split('/')[-1] for f in ssf['files']}
                        for cf in core_add:
                            if cf not in existing:
                                ssf['files'].append({'path': '../Core/Src/' + cf})
                                print(f'Added Core: {cf}')

# 3. Add HARDWARE groups
hw_groups = {
    'hardware': ['HARDWARE/TIMER/timer.c','HARDWARE/TIMER/delay.c','HARDWARE/TIMER/led.c',
                 'HARDWARE/TIMER/key.c','HARDWARE/TIMER/mpu.c','HARDWARE/TIMER/GBK_LibDrive.c',
                 'HARDWARE/MP3/mp3.c','system.c'],
    'LCD': ['HARDWARE/TIMER/FONT.c','HARDWARE/TIMER/Picture.c','HARDWARE/TIMER/Text.c',
            'HARDWARE/TIMER/Dis_Picture.c','HARDWARE/TIMER/lcd.c'],
    'ov2640': ['HARDWARE/TIMER/dcmi.c','HARDWARE/TIMER/stm32h7xx_hal_dcmi.c',
               'HARDWARE/TIMER/sccb.c','HARDWARE/TIMER/ov2640.c',
               'HARDWARE/TIMER/OV_Frame.c','HARDWARE/TIMER/usart2.c'],
}
app_folder = None
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Application':
        app_folder = folder
        break
if app_folder:
    for grp_name, paths in hw_groups.items():
        found = None
        for f in app_folder['folders']:
            if f['name'] == grp_name:
                found = f
                break
        if not found:
            new_grp = {'name': grp_name, 'files': [{'path': p} for p in paths], 'folders': []}
            app_folder['folders'].append(new_grp)
            print(f'Added group: {grp_name} ({len(paths)} files)')

# 4. Add LVGL files (minimal set needed)
lvgl_files = []
lvgl_base = '../Middlewares/Third_Party/LVGL/src'
# Core
for f in ['lv_init.c']:
    lvgl_files.append(f'{lvgl_base}/{f}')
for f in ['lv_group.c','lv_obj.c','lv_obj_class.c','lv_obj_draw.c','lv_obj_event.c',
          'lv_obj_id_builtin.c','lv_obj_pos.c','lv_obj_property.c','lv_obj_scroll.c',
          'lv_obj_style.c','lv_obj_style_gen.c','lv_obj_tree.c','lv_observer.c','lv_refr.c']:
    lvgl_files.append(f'{lvgl_base}/core/{f}')
# Display
lvgl_files.append(f'{lvgl_base}/display/lv_display.c')
# Draw
for f in ['lv_draw.c','lv_draw_arc.c','lv_draw_buf.c','lv_draw_image.c','lv_draw_label.c',
          'lv_draw_line.c','lv_draw_mask.c','lv_draw_rect.c','lv_draw_triangle.c','lv_image_decoder.c']:
    lvgl_files.append(f'{lvgl_base}/draw/{f}')
for f in ['lv_draw_sw_blend.c','lv_draw_sw_blend_to_rgb565.c']:
    lvgl_files.append(f'{lvgl_base}/draw/sw/blend/{f}')
for f in ['lv_draw_sw.c','lv_draw_sw_fill.c','lv_draw_sw_img.c','lv_draw_sw_letter.c',
          'lv_draw_sw_transform.c','lv_draw_sw_mask.c','lv_draw_sw_mask_rect.c',
          'lv_draw_sw_border.c','lv_draw_sw_box_shadow.c','lv_draw_sw_line.c',
          'lv_draw_sw_arc.c','lv_draw_sw_triangle.c','lv_draw_sw_grad.c','lv_draw_sw_utils.c']:
    lvgl_files.append(f'{lvgl_base}/draw/sw/{f}')
lvgl_files.append(f'{lvgl_base}/draw/convert/lv_draw_buf_convert.c')
# Font
for f in ['lv_font.c','lv_font_montserrat_14.c','lv_font_montserrat_16.c',
          'lv_font_montserrat_20.c','lv_font_montserrat_24.c','lv_font_fmt_txt.c']:
    lvgl_files.append(f'{lvgl_base}/font/{f}')
# Misc
for f in ['lv_anim.c','lv_area.c','lv_color.c','lv_color_op.c','lv_event.c',
          'lv_fs.c','lv_ll.c','lv_log.c','lv_lru.c','lv_math.c','lv_palette.c',
          'lv_style.c','lv_style_gen.c','lv_text.c','lv_text_ap.c','lv_timer.c',
          'lv_utils.c','lv_array.c','lv_async.c','lv_bidi.c','lv_iter.c']:
    lvgl_files.append(f'{lvgl_base}/misc/{f}')
# OSAL
lvgl_files.append(f'{lvgl_base}/osal/lv_os_none.c')
# STDLIB
for f in ['lv_mem_core_builtin.c','lv_sprintf_builtin.c','lv_string_builtin.c','lv_tlsf.c']:
    lvgl_files.append(f'{lvgl_base}/stdlib/builtin/{f}')
lvgl_files.append(f'{lvgl_base}/stdlib/lv_mem.c')
# Theme
lvgl_files.append(f'{lvgl_base}/themes/default/lv_theme_default.c')
lvgl_files.append(f'{lvgl_base}/themes/lv_theme.c')
# Tick
lvgl_files.append(f'{lvgl_base}/tick/lv_tick.c')
# Widgets
widgets = ['arc','bar','button','buttonmatrix','calendar','canvas','chart','checkbox',
           'dropdown','image','imagebutton','keyboard','label','led','line','list',
           'menu','msgbox','roller','scale','slider','span','spinbox','spinner',
           'switch','table','tabview','textarea','tileview','win']
for w in widgets:
    lvgl_files.append(f'{lvgl_base}/widgets/{w}/lv_{w}.c')
# Indev
for f in ['lv_indev.c','lv_indev_scroll.c']:
    lvgl_files.append(f'{lvgl_base}/indev/{f}')
# Layouts
lvgl_files.append(f'{lvgl_base}/layouts/lv_layout.c')
lvgl_files.append(f'{lvgl_base}/layouts/flex/lv_flex.c')
lvgl_files.append(f'{lvgl_base}/layouts/grid/lv_grid.c')
# Libs
lvgl_files.append(f'{lvgl_base}/libs/bin_decoder/lv_bin_decoder.c')

mid_folder = None
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Middlewares':
        mid_folder = folder
        break
if mid_folder:
    lvgl_found = None
    for f in mid_folder.get('folders', []):
        if f['name'] == 'LVGL':
            lvgl_found = f
            break
    if not lvgl_found:
        lvgl_found = {'name': 'LVGL', 'files': [], 'folders': []}
        mid_folder['folders'].append(lvgl_found)
    lvgl_found['files'] = [{'path': p} for p in lvgl_files]
    print(f'LVGL: {len(lvgl_files)} files')

# 5. Fix FreeRTOS CMSIS_RTOS
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Middlewares':
        for mf in folder.get('folders', []):
            if mf['name'] == 'FreeRTOS':
                new_files = []
                for f in mf['files']:
                    p = f['path']
                    if 'CMSIS_RTOS_V2' in p:
                        new_files.append({'path': '../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c'})
                    else:
                        new_files.append(f)
                mf['files'] = new_files
                print('FreeRTOS CMSIS_RTOS fixed')

# 6. Fix Lib path
for folder in config['virtualFolder']['folders']:
    if folder['name'] == 'Lib':
        folder['files'] = [{'path': '../Middlewares/ST/AI/Lib/NetworkRuntime1010_CM7_Keil.lib'}]

# 7. Include paths
for tname, tcfg in config['targets'].items():
    inc = tcfg['cppPreprocessAttrs']['incList']
    if '../Middlewares/ST/AI/Inc' not in inc:
        inc.append('../Middlewares/ST/AI/Inc')
    if '../Drivers/CMSIS/Include' not in inc:
        inc.insert(5, '../Drivers/CMSIS/Include')
    tcfg['cppPreprocessAttrs']['incList'] = inc

# 8. Name
config['name'] = 'finaltest'

with open('e:/base/finaltest/MDK-ARM/.eide/eide.yml', 'w', encoding='utf-8') as f:
    yaml.dump(config, f)
print('\nDone!')
