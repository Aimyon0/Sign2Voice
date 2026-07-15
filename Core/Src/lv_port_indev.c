/**
 * @file lv_port_indev.c
 * LVGL v9.x Input Device Port — skeleton for future touch support
 *
 * No touch controller is currently present on this hardware.
 * This file provides the structural skeleton and a placeholder read
 * callback.  When a touch controller driver is added, implement the
 * actual read logic inside `lv_port_indev_read()`.
 *
 * Typical touch controllers for this setup:
 *   - XPT2046 (resistive, SPI, commonly paired with ST7789V modules)
 *   - FT5x06 / FT6x06 (capacitive, I²C)
 *   - GT911 / GT1151 (capacitive, I²C)
 */

#include "lv_port_indev.h"
#include "main.h"

/* ================================================================
 *  Touch state placeholder
 * ================================================================ */

/* Example touch data structure — adapt to actual touch HW */
typedef struct {
    lv_indev_state_t state;   /* LV_INDEV_STATE_PR / LV_INDEV_STATE_REL */
    lv_point_t       point;   /* Last known touch coordinates */
} lv_port_touch_t;

static lv_port_touch_t touch_data;

/* ================================================================
 *  Read callback
 * ================================================================ */

/**
 * @brief  LVGL input device read callback.
 *
 * LVGL calls this periodically (default ~30 Hz) to poll the touch state.
 * Currently returns a "released / not pressed" state.
 *
 * When touch hardware is available, read the controller here and fill in
 * `touch_data.state` and `touch_data.point`.
 */
static void lv_port_indev_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    /*
     * TODO: When touch hardware is connected:
     *
     * 1. Poll the touch controller (e.g. XPT2046 via SPI, FT5x06 via I²C)
     * 2. If a touch is detected:
     *      touch_data.state = LV_INDEV_STATE_PR;
     *      touch_data.point.x = <raw X, possibly swap/mirror>;
     *      touch_data.point.y = <raw Y, possibly swap/mirror>;
     *    Else:
     *      touch_data.state = LV_INDEV_STATE_REL;
     *
     * 3. Perform any coordinate calibration (rotation, mirroring).
     */

    /* Placeholder: no touch pressed */
    touch_data.state = LV_INDEV_STATE_REL;

    data->state   = touch_data.state;
    data->point.x = touch_data.point.x;
    data->point.y = touch_data.point.y;
    data->continue_reading = 0;   /* No more points in this gesture */
}

/* ================================================================
 *  Public API
 * ================================================================ */

lv_indev_t *lv_port_indev_init(void)
{
    /* ---- Create an LVGL pointer-type input device ---- */
    lv_indev_t *indev = lv_indev_create();
    if (indev == NULL) {
        return NULL;
    }

    /* ---- Register the read callback ---- */
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lv_port_indev_read);

    /* ---- Optional: set cursor (can be NULL for touch) ---- */
    /* lv_indev_set_cursor(indev, NULL); */ /* default is NULL */

    return indev;
}
