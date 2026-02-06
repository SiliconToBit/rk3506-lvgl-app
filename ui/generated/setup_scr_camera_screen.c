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



void setup_scr_camera_screen(lv_ui *ui)
{
    //Write codes camera_screen
    ui->camera_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->camera_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->camera_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for camera_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->camera_screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes camera_screen_img_3
    ui->camera_screen_img_3 = lv_img_create(ui->camera_screen);
    lv_obj_add_flag(ui->camera_screen_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->camera_screen_img_3, &_photograph_alpha_100x100);
    lv_img_set_pivot(ui->camera_screen_img_3, 50,50);
    lv_img_set_angle(ui->camera_screen_img_3, 0);
    lv_obj_set_pos(ui->camera_screen_img_3, 730, 185);
    lv_obj_set_size(ui->camera_screen_img_3, 100, 100);

    //Write style for camera_screen_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->camera_screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->camera_screen_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->camera_screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->camera_screen_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes camera_screen_img_2
    ui->camera_screen_img_2 = lv_img_create(ui->camera_screen);
    lv_obj_add_flag(ui->camera_screen_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_pivot(ui->camera_screen_img_2, 50,50);
    lv_img_set_angle(ui->camera_screen_img_2, 0);
    lv_obj_set_pos(ui->camera_screen_img_2, 36, 25);
    lv_obj_set_size(ui->camera_screen_img_2, 667, 430);

    //Write style for camera_screen_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->camera_screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->camera_screen_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->camera_screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->camera_screen_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes camera_screen_img_1
    ui->camera_screen_img_1 = lv_img_create(ui->camera_screen);
    lv_obj_add_flag(ui->camera_screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->camera_screen_img_1, &_img_alpha_100x100);
    lv_img_set_pivot(ui->camera_screen_img_1, 50,50);
    lv_img_set_angle(ui->camera_screen_img_1, 0);
    lv_obj_set_pos(ui->camera_screen_img_1, 730, 334);
    lv_obj_set_size(ui->camera_screen_img_1, 100, 100);

    //Write style for camera_screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->camera_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->camera_screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->camera_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->camera_screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of camera_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->camera_screen);

    //Init events for screen.
    events_init_camera_screen(ui);
}
