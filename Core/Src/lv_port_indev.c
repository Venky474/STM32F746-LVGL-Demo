/*
 * lv_port_indev.c
 *
 *  Created on: Jan 31, 2026
 *      Author: PC
 */

#include "lvgl.h"
#include "lv_port_indev.h"
#include "stm32746g_discovery_ts.h"

static void touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data);

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;

    lv_indev_drv_register(&indev_drv);
}

static void touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    TS_StateTypeDef ts;

    BSP_TS_GetState(&ts);

    if(ts.touchDetected)
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = ts.touchX[0];
        data->point.y = ts.touchY[0];
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}
