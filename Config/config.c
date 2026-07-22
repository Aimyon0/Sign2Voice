#include "config.h"
#include "flash_storage.h"

static SystemConfig g_cfg __attribute__((aligned(32)));

ErrorCode config_init(void)
{
    ErrorCode e = config_load();
    if (e == ERR_OK && g_cfg.version == CONFIG_VERSION) return ERR_OK;
    config_reset();
    return ERR_OK;
}

ErrorCode config_load(void)
{
    SystemConfig tmp __attribute__((aligned(32)));
    ErrorCode e = flash_storage_read(&tmp, sizeof(tmp));
    if (e != ERR_OK) return e;
    if (tmp.volume > 30 || tmp.threshold < 50 || tmp.threshold > 95 ||
        (tmp.window_size != 3 && tmp.window_size != 5 && tmp.window_size != 7))
        return ERR_FLASH;
    g_cfg = tmp;
    return ERR_OK;
}

ErrorCode config_save(void)
{
    g_cfg.version = CONFIG_VERSION;
    return flash_storage_write(&g_cfg, sizeof(g_cfg));
}

void config_reset(void)
{
    g_cfg.volume = CONFIG_DEFAULT_VOLUME;
    g_cfg.threshold = CONFIG_DEFAULT_THRESHOLD;
    g_cfg.window_size = CONFIG_DEFAULT_WINDOW_SIZE;
    g_cfg.version = CONFIG_VERSION;
}

uint8_t config_get_volume(void)      { return g_cfg.volume; }
uint8_t config_get_threshold(void)   { return g_cfg.threshold; }
uint8_t config_get_window_size(void) { return g_cfg.window_size; }
void config_set_volume(uint8_t v)      { if (v <= 30) g_cfg.volume = v; }
void config_set_threshold(uint8_t t)   { if (t >= 50 && t <= 95) g_cfg.threshold = t; }
void config_set_window_size(uint8_t w) { if (w == 3 || w == 5 || w == 7) g_cfg.window_size = w; }
