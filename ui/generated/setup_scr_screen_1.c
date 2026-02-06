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



void setup_scr_screen_1(lv_ui *ui)
{
    //Write codes screen_1
    ui->screen_1 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_1, 854, 480);
    lv_obj_set_scrollbar_mode(ui->screen_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_weather_btn
    ui->screen_1_weather_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_weather_btn_label = lv_label_create(ui->screen_1_weather_btn);
    lv_label_set_text(ui->screen_1_weather_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_weather_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_weather_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_weather_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_weather_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_weather_btn, 366, 76);
    lv_obj_set_size(ui->screen_1_weather_btn, 120, 120);

    //Write style for screen_1_weather_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_weather_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_weather_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_weather_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_weather_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_weather_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_weather_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_weather_btn, &_weather_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_weather_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_weather_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_weather_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_weather_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_weather_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_weather_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_cpu_btn
    ui->screen_1_cpu_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_cpu_btn_label = lv_label_create(ui->screen_1_cpu_btn);
    lv_label_set_text(ui->screen_1_cpu_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_cpu_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_cpu_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_cpu_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_cpu_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_cpu_btn, 610, 76);
    lv_obj_set_size(ui->screen_1_cpu_btn, 120, 120);

    //Write style for screen_1_cpu_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_cpu_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_cpu_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_cpu_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_cpu_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_cpu_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_cpu_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_cpu_btn, &_cpu_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_cpu_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_cpu_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_cpu_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_cpu_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_cpu_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_cpu_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_setting_btn
    ui->screen_1_setting_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_setting_btn_label = lv_label_create(ui->screen_1_setting_btn);
    lv_label_set_text(ui->screen_1_setting_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_setting_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_setting_btn_label, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_setting_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_setting_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_setting_btn, 122, 76);
    lv_obj_set_size(ui->screen_1_setting_btn, 120, 120);

    //Write style for screen_1_setting_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_setting_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_setting_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_setting_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_setting_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_setting_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_setting_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_setting_btn, &_setting_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_setting_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_setting_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_setting_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_setting_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_setting_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_setting_btn, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_sensor_btn
    ui->screen_1_sensor_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_sensor_btn_label = lv_label_create(ui->screen_1_sensor_btn);
    lv_label_set_text(ui->screen_1_sensor_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_sensor_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_sensor_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_sensor_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_sensor_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_sensor_btn, 122, 288);
    lv_obj_set_size(ui->screen_1_sensor_btn, 120, 120);

    //Write style for screen_1_sensor_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_sensor_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_sensor_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_sensor_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_sensor_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_sensor_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_sensor_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_sensor_btn, &_sensor_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_sensor_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_sensor_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_sensor_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_sensor_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_sensor_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_sensor_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_btn
    ui->screen_1_img_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_img_btn_label = lv_label_create(ui->screen_1_img_btn);
    lv_label_set_text(ui->screen_1_img_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_img_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_img_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_img_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_img_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_img_btn, 366, 288);
    lv_obj_set_size(ui->screen_1_img_btn, 120, 120);

    //Write style for screen_1_img_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_img_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_img_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_img_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_img_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_img_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_img_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_img_btn, &_img_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_img_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_img_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_img_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_img_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_img_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_img_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_music_btn
    ui->screen_1_music_btn = lv_btn_create(ui->screen_1);
    ui->screen_1_music_btn_label = lv_label_create(ui->screen_1_music_btn);
    lv_label_set_text(ui->screen_1_music_btn_label, "");
    lv_label_set_long_mode(ui->screen_1_music_btn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_music_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_music_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_music_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->screen_1_music_btn, 610, 288);
    lv_obj_set_size(ui->screen_1_music_btn, 120, 120);

    //Write style for screen_1_music_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_music_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_music_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_music_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_music_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_music_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_music_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->screen_1_music_btn, &_music_120x120, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->screen_1_music_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->screen_1_music_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_music_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_music_btn, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_music_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_music_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_1.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_1);

    //Init events for screen.
    events_init_screen_1(ui);
}
