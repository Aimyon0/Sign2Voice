#include "menu.h"
#include "lvgl.h"
#include "key.h"
#include "mp3.h"
#include "FreeRTOS.h"
#include <stdio.h>

static menu_page_t g_page = MENU_RECOGNIZE;
static int g_vol = 20, g_thr = 70, g_bri = 5;
static int g_main_sel = 0, g_sett_sel = 0;
static uint32_t g_fps = 0, g_inf_ms = 0, g_free_heap = 0;
static uint8_t  g_last_raw = 0;

/* Colors */
#define C_BG    lv_color_hex(0x0B0D10)
#define C_HDR   lv_color_hex(0x1A1C20)
#define C_SEP   lv_color_hex(0x303030)
#define C_TXT   lv_color_hex(0xF0F0F0)
#define C_HL    lv_color_hex(0xFFD54A)
#define C_DIM   lv_color_hex(0x909090)
#define C_OK    lv_color_hex(0x00D26A)

/* Screens */
static lv_obj_t *scr_main, *scr_gesture, *scr_settings, *scr_status, *scr_about;
static lv_obj_t *scr_vol, *scr_thr, *scr_bright;
static lv_obj_t *lbl_vol_val, *lbl_thr_val, *lbl_bri_val;
static lv_obj_t *bar_vol_inner, *bar_thr_inner, *bar_bri_inner;
static lv_obj_t *lbl_main_items[5], *lbl_sett_items[3];
static const char *g_main_texts[5], *g_sett_texts[3];
static lv_obj_t *lbl_st_fps, *lbl_st_inf, *lbl_st_heap, *lbl_st_temp;
extern lv_obj_t *ui_scr_orig;

/* CPU temperature via ADC3 channel 18 */
#ifndef __IO
#define __IO volatile
#endif

// 声明外部 OS 延时函数（根据你的 OS 实际 API 决定，FreeRTOS 通常是 vTaskDelay 或 osDelay）
extern void osDelay(uint32_t ticks);

static uint8_t g_temp_ok = 0;

static void temp_init(void)
{
    /* 1. 使能 ADC3 时钟 */
    __HAL_RCC_ADC3_CLK_ENABLE();

    /* 2. 将 ADC 时钟限制到最慢 (异步时钟模式下 PCLK / 4) */
    MODIFY_REG(ADC3_COMMON->CCR, ADC_CCR_CKMODE, ADC_CCR_CKMODE_0 | ADC_CCR_CKMODE_1);

    /* 3. 使能温度传感器 + Vref */
    ADC3_COMMON->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN;

    /* 4. 打开 ADC 电源 */
    ADC3->CR &= ~ADC_CR_DEEPPWD;
    ADC3->CR |= ADC_CR_ADVREGEN;

    /* 初始化阶段可能 OS 内核还未启动，使用大硬件循环确保电压彻底稳定 */
    for (volatile int i = 0; i < 2000000; i++); 

    /* 5. 校准 */
    ADC3->CR |= ADC_CR_ADCAL;
    while (ADC3->CR & ADC_CR_ADCAL);

    /* 6. 使能 ADC */
    ADC3->CR |= ADC_CR_ADEN;
    while (!(ADC3->ISR & ADC_ISR_ADRDY));

    /* 7. 关键：内部温度传感器（TSEN）电容充电需要较长时间 */
    for (volatile int i = 0; i < 5000000; i++);

    g_temp_ok = 1;
}

static int temp_read_int(void)
{
    // 如果没有初始化，由于此时肯定在任务中运行，可以使用 osDelay
    if (!g_temp_ok) {
        temp_init();
        osDelay(20); // 给 OS 任务让出时间，等待传感器稳定
    }

    /* ===== 1. 读取 16 位工厂校准值 ===== */
    uint16_t TS_CAL1 = *(__IO uint16_t*)0x1FF1E820; 
    uint16_t TS_CAL2 = *(__IO uint16_t*)0x1FF1E840; 

    if (TS_CAL1 == 0 || TS_CAL2 == 0 || TS_CAL2 <= TS_CAL1) {
        return -999; 
    }

    /* ===== 2. 配置 ADC3 通道 18 和 16位分辨率 ===== */
    ADC3->PCSEL |= (1U << 18);
    ADC3->SQR1 &= ~(ADC_SQR1_L | (0x1FU << 6)); 
    ADC3->SQR1 |= (18U << 6); 

    // 确保是 16-bit 满分辨率
    ADC3->CFGR &= ~(7U << 2); 

    // 配置最大采样时间 (810.5 cycles) 确保阻抗匹配
    ADC3->SMPR2 &= ~(7U << 24); 
    ADC3->SMPR2 |= (7U << 24);  

    ADC3->CFGR &= ~ADC_CFGR_CONT;

    /* ===== 3. 丢弃前两次采样，让流水线排空 ===== */
    for(int i = 0; i < 2; i++) {
        ADC3->CR |= ADC_CR_ADSTART;
        while (!(ADC3->ISR & ADC_ISR_EOC));
        (void)ADC3->DR;
        osDelay(1); // 每次丢弃释放 1 个 tick 的时间
    }

    /* ===== 4. 多次采样求平均 ===== */
    uint32_t sum = 0;
    const int N = 16;
    for (int i = 0; i < N; i++) {
        ADC3->CR |= ADC_CR_ADSTART;
        while (!(ADC3->ISR & ADC_ISR_EOC));
        sum += ADC3->DR;
        
        // 降低 CPU 占用，给硬件采样过渡时间
        for (volatile int d = 0; d < 10000; d++);
    }
    uint32_t avg = sum / N;

    /* ===== 5. 纯整数运算 ===== */
    int32_t temp_x10 = ((int32_t)avg - (int32_t)TS_CAL1) * 800 / (int32_t)(TS_CAL2 - TS_CAL1) + 300;

    return (int)temp_x10;
}
/* ====== Helpers ====== */
static lv_obj_t *mk_screen(void) {
    lv_obj_t *s = lv_obj_create(NULL);
    lv_obj_remove_style_all(s);
    lv_obj_set_style_bg_color(s, C_BG, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(s, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(s, 0, 0);
    lv_obj_set_style_border_width(s, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return s;
}
static void mk_hdr(lv_obj_t *sc, const char *t) {
    lv_obj_t *h = lv_obj_create(sc);
    lv_obj_remove_style_all(h);
    lv_obj_set_size(h, 240, 26); lv_obj_set_pos(h, 0, 0);
    lv_obj_set_style_bg_color(h, C_HDR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(h, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(h, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(h, 0, 0);
    lv_obj_set_style_pad_all(h, 0, 0);
    lv_obj_t *l = lv_label_create(h); lv_label_set_text(l, t);
    lv_obj_set_style_text_color(l, C_TXT, 0); lv_obj_center(l);
    /* separator line below header */
    lv_obj_t *sep = lv_obj_create(sc); lv_obj_remove_style_all(sep);
    lv_obj_set_style_bg_color(sep, C_SEP, 0); lv_obj_set_style_bg_opa(sep, LV_OPA_COVER, 0);
    lv_obj_set_size(sep, 240, 1); lv_obj_set_pos(sep, 0, 26);
}
static void mk_ftr(lv_obj_t *sc, const char *k1, const char *k2) {
    lv_obj_t *a = lv_label_create(sc); lv_label_set_text_fmt(a, "KEY1 %s", k1);
    lv_obj_set_style_text_color(a, C_DIM, 0); lv_obj_set_pos(a, 6, 290);
    lv_obj_t *b = lv_label_create(sc); lv_label_set_text_fmt(b, "KEY2 %s", k2);
    lv_obj_set_style_text_color(b, C_DIM, 0); lv_obj_set_pos(b, 120, 290);
}
static lv_obj_t *mk_item(lv_obj_t *p, int y, const char *t) {
    lv_obj_t *l = lv_label_create(p); lv_label_set_text(l, t);
    lv_obj_set_style_text_color(l, C_TXT, 0); lv_obj_set_pos(l, 30, y); return l;
}
static void highlight(lv_obj_t *items[], int n, int sel, const char *txts[]) {
    for (int i = 0; i < n; i++) {
        lv_obj_set_style_text_color(items[i], (i==sel)?C_HL:C_TXT, 0);
        lv_label_set_text_fmt(items[i], "%s %s", (i==sel)?">":" ", txts[i]);
    }
}
static lv_obj_t *mk_bar(lv_obj_t *p, int y) {
    lv_obj_t *b = lv_obj_create(p);
    lv_obj_remove_style_all(b);
    lv_obj_set_size(b, 220, 14); lv_obj_set_pos(b, 10, y);
    lv_obj_set_style_bg_color(b, C_SEP, 0);
    lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(b, 0, 0);
    lv_obj_set_style_pad_all(b, 0, 0);
    return b;
}
static lv_obj_t *mk_bar_inner(lv_obj_t *bar) {
    lv_obj_t *in = lv_obj_create(bar);
    lv_obj_remove_style_all(in);
    lv_obj_set_style_bg_color(in, C_HL, 0);
    lv_obj_set_style_bg_opa(in, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(in, 0, 0);
    lv_obj_set_style_pad_all(in, 0, 0);
    lv_obj_set_height(in, 14); lv_obj_set_pos(in, 0, 0);
    return in;
}

/* ====== Init ====== */
void menu_init(void) {
    lv_disp_t *disp = lv_disp_get_default();
    if (disp) lv_disp_set_theme(disp, NULL);
    scr_main = mk_screen();
    mk_hdr(scr_main, "Sign2Voice   v1.0");
    mk_ftr(scr_main, "Select", "Next");
    const char *m[] = {"Recognition","Gesture Library","Settings","System Status","About"};
    for (int i = 0; i < 5; i++) { g_main_texts[i]=m[i]; lbl_main_items[i]=mk_item(scr_main, 38+i*38, m[i]); }
    highlight(lbl_main_items, 5, 0, g_main_texts);
    scr_gesture=scr_settings=scr_status=scr_about=NULL; scr_vol=scr_thr=scr_bright=NULL;
}

/* ====== On-demand factories ====== */
static void f_gesture(void) {
    scr_gesture=mk_screen(); mk_hdr(scr_gesture,"Gesture Library"); mk_ftr(scr_gesture,"--","Back");
    mk_item(scr_gesture, 40, "Supported Gestures");
    mk_item(scr_gesture, 72,"* Fist"); mk_item(scr_gesture, 98,"* Palm");
    mk_item(scr_gesture, 124,"* OK"); mk_item(scr_gesture, 150,"* Like");
    lv_obj_t *g=mk_item(scr_gesture,185,"No Gesture = mute"); lv_obj_set_style_text_color(g,C_DIM,0);
}
static void f_settings(void) {
    scr_settings=mk_screen(); mk_hdr(scr_settings,"Settings"); mk_ftr(scr_settings,"Enter","Back");
    const char *s[]={"Speaker Volume","Confidence","Window Size"};
    for(int i=0;i<3;i++){g_sett_texts[i]=s[i];lbl_sett_items[i]=mk_item(scr_settings,40+i*48,s[i]);}
    highlight(lbl_sett_items,3,0,g_sett_texts);
}
static void f_vol(void) {
    scr_vol=mk_screen(); mk_hdr(scr_vol,"Speaker Volume"); mk_ftr(scr_vol,"+","-");
    lbl_vol_val=mk_item(scr_vol,60,"20 / 30"); lv_obj_set_style_text_color(lbl_vol_val,C_HL,0);
    lv_obj_t *bx=mk_bar(scr_vol,92); bar_vol_inner=mk_bar_inner(bx);
    lv_obj_set_width(bar_vol_inner,20*220/30);
}
static void f_thr(void) {
    scr_thr=mk_screen(); mk_hdr(scr_thr,"Confidence"); mk_ftr(scr_thr,"+","-");
    lbl_thr_val=mk_item(scr_thr,60,"70 %"); lv_obj_set_style_text_color(lbl_thr_val,C_HL,0);
    lv_obj_t *bx=mk_bar(scr_thr,92); bar_thr_inner=mk_bar_inner(bx);
    lv_obj_set_width(bar_thr_inner,70*220/100);
    mk_item(scr_thr,120,"Lower = more sensitive");
}
static void f_bright(void) {
    scr_bright=mk_screen(); mk_hdr(scr_bright,"Window Size"); mk_ftr(scr_bright,"+","-");
    mk_item(scr_bright,44,"Low    3 frames"); mk_item(scr_bright,86,"Medium  5 frames"); mk_item(scr_bright,128,"High    7 frames");
    lbl_bri_val=mk_item(scr_bright,170,"5 frames"); lv_obj_set_style_text_color(lbl_bri_val,C_HL,0);
}
static void f_status(void)
{
    scr_status = mk_screen();
    mk_hdr(scr_status, "System Status");
    mk_ftr(scr_status, "--", "Back");

    lv_obj_t *c1 = mk_item(scr_status, 40,  "Camera        Ready");
    lv_obj_set_style_text_color(c1, C_OK, 0);

    lv_obj_t *c2 = mk_item(scr_status, 70,  "AI Engine     Running");
    lv_obj_set_style_text_color(c2, C_OK, 0);

    lbl_st_temp = mk_item(scr_status,100, "CPU Temp      --.- C");
    lbl_st_fps  = mk_item(scr_status,130, "FPS           --");
    lbl_st_inf  = mk_item(scr_status,160, "Inference     -- ms");
    lbl_st_heap = mk_item(scr_status,190, "Free Heap     -- KB");
}
static void f_about(void) {
    scr_about=mk_screen(); mk_hdr(scr_about,"About"); mk_ftr(scr_about,"--","Back");
    mk_item(scr_about,44,"Sign2Voice"); mk_item(scr_about,76,"AI Gesture Recognition");
    lv_obj_t *a1=mk_item(scr_about,112,"MCU  STM32H743VIT6"); lv_obj_set_style_text_color(a1,C_DIM,0);
    lv_obj_t *a2=mk_item(scr_about,142,"OS   FreeRTOS + LVGL"); lv_obj_set_style_text_color(a2,C_DIM,0);
    lv_obj_t *a3=mk_item(scr_about,172,"AI    X-CUBE-AI"); lv_obj_set_style_text_color(a3,C_DIM,0);
    lv_obj_t *a4=mk_item(scr_about,210,"v1.0  (c) 2026"); lv_obj_set_style_text_color(a4,C_DIM,0);
}

/* ====== Page switch ====== */
void menu_enter(menu_page_t page) {
    g_page = page;
    lv_obj_t *scr = NULL;
    switch(page) {
    case MENU_RECOGNIZE: lv_screen_load(ui_scr_orig); return;
    case MENU_MAIN:      scr = scr_main; break;
    case MENU_GESTURE:   if(!scr_gesture) f_gesture(); scr = scr_gesture; break;
    case MENU_SETTINGS:  if(!scr_settings)f_settings();scr = scr_settings;break;
    case MENU_VOLUME:    if(!scr_vol) f_vol(); scr = scr_vol; break;
    case MENU_THRESHOLD: if(!scr_thr) f_thr(); scr = scr_thr; break;
    case MENU_BRIGHT:    if(!scr_bright)f_bright();scr = scr_bright;break;
    case MENU_STATUS:    if(!scr_status) f_status();
        if(lbl_st_fps) lv_label_set_text_fmt(lbl_st_fps,"FPS       %lu",g_fps);
        if(lbl_st_inf) lv_label_set_text_fmt(lbl_st_inf,"Inference %lu ms",g_inf_ms);
        if(lbl_st_heap)lv_label_set_text_fmt(lbl_st_heap,"Free Heap %lu KB",g_free_heap/1024);
        if(lbl_st_temp)
        {int t_x10 = temp_read_int();
    
    if (t_x10 == -999) {
        lv_label_set_text(lbl_st_temp, "CPU Temp: CAL ERR");
    } 
    else if (t_x10 < -400 || t_x10 > 1250) {
        // 如果硬件读出全是 0 或 0xFFFF，算出来的整数会严重越界
        // 此时我们直接把硬件读出的错误数据打印出来看看到底是多少
        lv_label_set_text_fmt(lbl_st_temp, "CPU Temp: RANGE ERR (%d)", t_x10);
    } 
    else {
        // 提取整数部分和小数部分，用 %d.%d 打印
        int integer_part = t_x10 / 10;
        int decimal_part = t_x10 % 10;
        if (decimal_part < 0) decimal_part = -decimal_part; // 负数处理

        lv_label_set_text_fmt(lbl_st_temp, "CPU Temp: %d.%d C", integer_part, decimal_part);
    }
        }

        scr = scr_status; break;
    case MENU_ABOUT:     if(!scr_about) f_about(); scr = scr_about; break;
    }
    if (scr) { lv_screen_load(scr); lv_obj_set_style_bg_color(scr, C_BG, 0); lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0); }
}

int menu_is_recognize(void){return g_page==MENU_RECOGNIZE;}
void menu_set_fps(uint32_t fps){g_fps=fps;}
void menu_set_stats(uint32_t f,uint32_t i,uint32_t h){g_fps=f;g_inf_ms=i;g_free_heap=h;}
int menu_get_volume(void){return g_vol;}
int menu_get_threshold(void){return g_thr;}
int menu_get_brightness(void){return g_bri;}

/* ====== Keys ====== */
static int k2d(void){return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5)==GPIO_PIN_RESET;}
static uint32_t g_k2t=0; static uint8_t g_k2w=0;

void menu_process_key(uint8_t raw) {
    uint8_t k1=(raw==1)&&(g_last_raw!=1), k2=(raw==2)&&(g_last_raw!=2); g_last_raw=raw;
    int h=k2d(); if(h&&!g_k2w){g_k2t=HAL_GetTick();g_k2w=1;} if(!h)g_k2w=0;
    uint8_t kl=h&&g_k2w&&(HAL_GetTick()-g_k2t>800);
    if(!k1&&!k2&&!kl)return;

    switch(g_page){
    case MENU_RECOGNIZE: if(kl)menu_enter(MENU_MAIN); break;
    case MENU_MAIN:
        if(k2){g_main_sel=(g_main_sel+1)%5;if(lbl_main_items[0])highlight(lbl_main_items,5,g_main_sel,g_main_texts);}
        if(k1){static const menu_page_t d[5]={MENU_RECOGNIZE,MENU_GESTURE,MENU_SETTINGS,MENU_STATUS,MENU_ABOUT};menu_enter(d[g_main_sel]);}
        break;
    case MENU_GESTURE:case MENU_STATUS:case MENU_ABOUT: if(kl)menu_enter(MENU_MAIN); break;
    case MENU_SETTINGS:
        if(k2){g_sett_sel=(g_sett_sel+1)%3;if(lbl_sett_items[0])highlight(lbl_sett_items,3,g_sett_sel,g_sett_texts);}
        if(kl)menu_enter(MENU_MAIN);
        if(k1){static const menu_page_t s[3]={MENU_VOLUME,MENU_THRESHOLD,MENU_BRIGHT};menu_enter(s[g_sett_sel]);}
        break;
    case MENU_VOLUME:
        if(k1){g_vol++;if(g_vol>30)g_vol=0;MP3_SetVolume(g_vol);}
        if(k2){if(g_vol>0)g_vol--;else g_vol=30;MP3_SetVolume(g_vol);}
        if(kl)menu_enter(MENU_SETTINGS);
        if(lbl_vol_val)lv_label_set_text_fmt(lbl_vol_val,"%d / 30",g_vol);
        if(bar_vol_inner)lv_obj_set_width(bar_vol_inner,g_vol*220/30);
        break;
    case MENU_THRESHOLD:
        if(k1){g_thr+=5;if(g_thr>95)g_thr=50;}
        if(k2){g_thr-=5;if(g_thr<50)g_thr=95;}
        if(kl)menu_enter(MENU_SETTINGS);
        if(lbl_thr_val)lv_label_set_text_fmt(lbl_thr_val,"%d %%",g_thr);
        if(bar_thr_inner)lv_obj_set_width(bar_thr_inner,g_thr*220/100);
        break;
    case MENU_BRIGHT: {
        static const int wins[]={3,5,7}; static const char* wn[]={"3 frames","5 frames","7 frames"};
        static int wi=1;
        if(k1){wi=(wi+1)%3;g_bri=wins[wi];}
        if(k2){wi=(wi+2)%3;g_bri=wins[wi];}
        if(kl)menu_enter(MENU_SETTINGS);
        if(lbl_bri_val)lv_label_set_text(lbl_bri_val,wn[wi]);
        break; }
    }
}
