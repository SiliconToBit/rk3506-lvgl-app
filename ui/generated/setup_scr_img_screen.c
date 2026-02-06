/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_img_screen(lv_ui *ui)
{
    //Write codes img_screen
    ui->img_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->img_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->img_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for img_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->img_screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //The custom code of img_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->img_screen);

    //Init events for screen.
    events_init_img_screen(ui);
}
