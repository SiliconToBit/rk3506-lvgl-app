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



lv_calendar_date_t weather_screen_calendar_today;
lv_calendar_date_t weather_screen_calendar_highlihted_days[1];
void setup_scr_weather_screen(lv_ui *ui)
{
    //Write codes weather_screen
    ui->weather_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->weather_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->weather_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for weather_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->weather_screen, 201, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_cont_1
    ui->weather_screen_cont_1 = lv_obj_create(ui->weather_screen);
    lv_obj_set_pos(ui->weather_screen_cont_1, 13, 12);
    lv_obj_set_size(ui->weather_screen_cont_1, 826, 194);
    lv_obj_set_scrollbar_mode(ui->weather_screen_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for weather_screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->weather_screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->weather_screen_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->weather_screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_cont_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_cont_1, 180, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_cont_1, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_time_label
    ui->weather_screen_time_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_time_label, "12:00");
    lv_label_set_long_mode(ui->weather_screen_time_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_time_label, 590, 9);
    lv_obj_set_size(ui->weather_screen_time_label, 211, 68);

    //Write style for weather_screen_time_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_time_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_time_label, &lv_font_puhui_64, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_time_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_time_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_time_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_date_label
    ui->weather_screen_date_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_date_label, "2025-12-11 星期四");
    lv_label_set_long_mode(ui->weather_screen_date_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_date_label, 583, 88);
    lv_obj_set_size(ui->weather_screen_date_label, 230, 24);

    //Write style for weather_screen_date_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_date_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_date_label, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_date_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_date_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_date_label, 208, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_date_label, lv_color_hex(0xbc4a00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_date_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_date_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_location_label
    ui->weather_screen_location_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_location_label, "广州番禺");
    lv_label_set_long_mode(ui->weather_screen_location_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_location_label, 670, 134);
    lv_obj_set_size(ui->weather_screen_location_label, 128, 32);

    //Write style for weather_screen_location_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_location_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_location_label, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_location_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_location_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_location_label, 208, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_location_label, lv_color_hex(0xa60019), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_location_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_location_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_weather_text_label
    ui->weather_screen_weather_text_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_weather_text_label, "大暴雨到特大暴雨");
    lv_label_set_long_mode(ui->weather_screen_weather_text_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_weather_text_label, 183, 110);
    lv_obj_set_size(ui->weather_screen_weather_text_label, 192, 24);

    //Write style for weather_screen_weather_text_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_weather_text_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_weather_text_label, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_weather_text_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_weather_text_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_weather_text_label, 153, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_weather_text_label, lv_color_hex(0x00d9ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_weather_text_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_weather_text_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_img_1
    ui->weather_screen_img_1 = lv_img_create(ui->weather_screen_cont_1);
    lv_obj_add_flag(ui->weather_screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->weather_screen_img_1, &_location_alpha_50x50);
    lv_img_set_pivot(ui->weather_screen_img_1, 50,50);
    lv_img_set_angle(ui->weather_screen_img_1, 0);
    lv_obj_set_pos(ui->weather_screen_img_1, 605, 126);
    lv_obj_set_size(ui->weather_screen_img_1, 50, 50);

    //Write style for weather_screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->weather_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->weather_screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->weather_screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_winddir_label
    ui->weather_screen_winddir_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_winddir_label, "东风二级");
    lv_label_set_long_mode(ui->weather_screen_winddir_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_winddir_label, 276, 54);
    lv_obj_set_size(ui->weather_screen_winddir_label, 96, 24);

    //Write style for weather_screen_winddir_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_winddir_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_winddir_label, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_winddir_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_winddir_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_temp_label
    ui->weather_screen_temp_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_temp_label, "26℃");
    lv_label_set_long_mode(ui->weather_screen_temp_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_temp_label, 183, 54);
    lv_obj_set_size(ui->weather_screen_temp_label, 60, 24);

    //Write style for weather_screen_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_temp_label, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_temp_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_weather_icon_label
    ui->weather_screen_weather_icon_label = lv_label_create(ui->weather_screen_cont_1);
    lv_label_set_text(ui->weather_screen_weather_icon_label, "");
    lv_label_set_long_mode(ui->weather_screen_weather_icon_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_weather_icon_label, 21, 32);
    lv_obj_set_size(ui->weather_screen_weather_icon_label, 128, 128);

    //Write style for weather_screen_weather_icon_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_weather_icon_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_weather_icon_label, &lv_font_qweather_icons_128, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_weather_icon_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_weather_icon_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_cont_2
    ui->weather_screen_cont_2 = lv_obj_create(ui->weather_screen);
    lv_obj_set_pos(ui->weather_screen_cont_2, 17, 225);
    lv_obj_set_size(ui->weather_screen_cont_2, 335, 237);
    lv_obj_set_scrollbar_mode(ui->weather_screen_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for weather_screen_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->weather_screen_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->weather_screen_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->weather_screen_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_cont_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_cont_2, 180, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_cont_2, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day3_temp_label
    ui->weather_screen_day3_temp_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day3_temp_label, "24℃-26℃");
    lv_label_set_long_mode(ui->weather_screen_day3_temp_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day3_temp_label, 130, 191);
    lv_obj_set_size(ui->weather_screen_day3_temp_label, 90, 16);

    //Write style for weather_screen_day3_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day3_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day3_temp_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day3_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day3_temp_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day3_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_5
    ui->weather_screen_label_5 = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_label_5, "3日天气预报");
    lv_label_set_long_mode(ui->weather_screen_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_5, -4, 8);
    lv_obj_set_size(ui->weather_screen_label_5, 112, 21);

    //Write style for weather_screen_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_5, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_5, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_line_1
    ui->weather_screen_line_1 = lv_line_create(ui->weather_screen_cont_2);
    static lv_point_t weather_screen_line_1[] = {{0, 0},{331, 0},};
    lv_line_set_points(ui->weather_screen_line_1, weather_screen_line_1, 2);
    lv_obj_set_pos(ui->weather_screen_line_1, 0, 36);
    lv_obj_set_size(ui->weather_screen_line_1, 335, 3);

    //Write style for weather_screen_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->weather_screen_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->weather_screen_line_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->weather_screen_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->weather_screen_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_6
    ui->weather_screen_label_6 = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_label_6, "今天");
    lv_label_set_long_mode(ui->weather_screen_label_6, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_6, 10, 64);
    lv_obj_set_size(ui->weather_screen_label_6, 32, 16);

    //Write style for weather_screen_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_6, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_7
    ui->weather_screen_label_7 = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_label_7, "明天");
    lv_label_set_long_mode(ui->weather_screen_label_7, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_7, 10, 126);
    lv_obj_set_size(ui->weather_screen_label_7, 32, 16);

    //Write style for weather_screen_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_7, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_8
    ui->weather_screen_label_8 = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_label_8, "后天");
    lv_label_set_long_mode(ui->weather_screen_label_8, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_8, 10, 190);
    lv_obj_set_size(ui->weather_screen_label_8, 32, 16);

    //Write style for weather_screen_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_8, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day1_temp_label
    ui->weather_screen_day1_temp_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day1_temp_label, "24℃-26℃");
    lv_label_set_long_mode(ui->weather_screen_day1_temp_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day1_temp_label, 130, 62);
    lv_obj_set_size(ui->weather_screen_day1_temp_label, 90, 16);

    //Write style for weather_screen_day1_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day1_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day1_temp_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day1_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day1_temp_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day1_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day2_temp_label
    ui->weather_screen_day2_temp_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day2_temp_label, "24℃-26℃");
    lv_label_set_long_mode(ui->weather_screen_day2_temp_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day2_temp_label, 129, 120);
    lv_obj_set_size(ui->weather_screen_day2_temp_label, 90, 16);

    //Write style for weather_screen_day2_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day2_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day2_temp_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day2_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day2_temp_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day2_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day1_winddir_label
    ui->weather_screen_day1_winddir_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day1_winddir_label, "南风");
    lv_label_set_long_mode(ui->weather_screen_day1_winddir_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day1_winddir_label, 231, 62);
    lv_obj_set_size(ui->weather_screen_day1_winddir_label, 96, 16);

    //Write style for weather_screen_day1_winddir_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day1_winddir_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day1_winddir_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day1_winddir_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day1_winddir_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day1_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day2_winddir_label
    ui->weather_screen_day2_winddir_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day2_winddir_label, "南风");
    lv_label_set_long_mode(ui->weather_screen_day2_winddir_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day2_winddir_label, 231, 121);
    lv_obj_set_size(ui->weather_screen_day2_winddir_label, 96, 16);

    //Write style for weather_screen_day2_winddir_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day2_winddir_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day2_winddir_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day2_winddir_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day2_winddir_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day2_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day3_winddir_label
    ui->weather_screen_day3_winddir_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day3_winddir_label, "南风");
    lv_label_set_long_mode(ui->weather_screen_day3_winddir_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day3_winddir_label, 231, 188);
    lv_obj_set_size(ui->weather_screen_day3_winddir_label, 96, 16);

    //Write style for weather_screen_day3_winddir_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day3_winddir_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day3_winddir_label, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day3_winddir_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day3_winddir_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day3_winddir_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day1_weather_icon_label
    ui->weather_screen_day1_weather_icon_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day1_weather_icon_label, "");
    lv_label_set_long_mode(ui->weather_screen_day1_weather_icon_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day1_weather_icon_label, 68, 56);
    lv_obj_set_size(ui->weather_screen_day1_weather_icon_label, 32, 32);

    //Write style for weather_screen_day1_weather_icon_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day1_weather_icon_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day1_weather_icon_label, &lv_font_qweather_icons_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day1_weather_icon_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day1_weather_icon_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day1_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day2_weather_icon_label
    ui->weather_screen_day2_weather_icon_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day2_weather_icon_label, "");
    lv_label_set_long_mode(ui->weather_screen_day2_weather_icon_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day2_weather_icon_label, 68, 117);
    lv_obj_set_size(ui->weather_screen_day2_weather_icon_label, 32, 32);

    //Write style for weather_screen_day2_weather_icon_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day2_weather_icon_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day2_weather_icon_label, &lv_font_qweather_icons_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day2_weather_icon_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day2_weather_icon_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day2_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_day3_weather_icon_label
    ui->weather_screen_day3_weather_icon_label = lv_label_create(ui->weather_screen_cont_2);
    lv_label_set_text(ui->weather_screen_day3_weather_icon_label, "");
    lv_label_set_long_mode(ui->weather_screen_day3_weather_icon_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_day3_weather_icon_label, 68, 187);
    lv_obj_set_size(ui->weather_screen_day3_weather_icon_label, 32, 32);

    //Write style for weather_screen_day3_weather_icon_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_day3_weather_icon_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_day3_weather_icon_label, &lv_font_qweather_icons_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_day3_weather_icon_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_day3_weather_icon_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_day3_weather_icon_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_calendar
    ui->weather_screen_calendar = lv_calendar_create(ui->weather_screen);
    weather_screen_calendar_today.year = 2025;
    weather_screen_calendar_today.month = 12;
    weather_screen_calendar_today.day = 20;
    lv_calendar_set_today_date(ui->weather_screen_calendar, weather_screen_calendar_today.year, weather_screen_calendar_today.month, weather_screen_calendar_today.day);
    lv_calendar_set_showed_date(ui->weather_screen_calendar, weather_screen_calendar_today.year, weather_screen_calendar_today.month);
    weather_screen_calendar_highlihted_days[0].year = 2025;
    weather_screen_calendar_highlihted_days[0].month = 12;
    weather_screen_calendar_highlihted_days[0].day = 21;
    lv_calendar_set_highlighted_dates(ui->weather_screen_calendar, weather_screen_calendar_highlihted_days, 1);
    lv_obj_t *weather_screen_calendar_header = lv_calendar_header_arrow_create(ui->weather_screen_calendar);
    lv_calendar_t *weather_screen_calendar = (lv_calendar_t *)ui->weather_screen_calendar;
    lv_obj_add_event_cb(weather_screen_calendar->btnm, weather_screen_calendar_draw_part_begin_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(ui->weather_screen_calendar, weather_screen_calendar_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(ui->weather_screen_calendar, 529, 225);
    lv_obj_set_size(ui->weather_screen_calendar, 311, 237);

    //Write style state: LV_STATE_DEFAULT for &style_weather_screen_calendar_main_main_default
    static lv_style_t style_weather_screen_calendar_main_main_default;
    ui_init_style(&style_weather_screen_calendar_main_main_default);

    lv_style_set_border_width(&style_weather_screen_calendar_main_main_default, 1);
    lv_style_set_border_opa(&style_weather_screen_calendar_main_main_default, 255);
    lv_style_set_border_color(&style_weather_screen_calendar_main_main_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_weather_screen_calendar_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_opa(&style_weather_screen_calendar_main_main_default, 179);
    lv_style_set_bg_color(&style_weather_screen_calendar_main_main_default, lv_color_hex(0x2F92DA));
    lv_style_set_bg_grad_dir(&style_weather_screen_calendar_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_shadow_width(&style_weather_screen_calendar_main_main_default, 0);
    lv_style_set_radius(&style_weather_screen_calendar_main_main_default, 0);
    lv_obj_add_style(ui->weather_screen_calendar, &style_weather_screen_calendar_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_weather_screen_calendar_extra_header_main_default
    static lv_style_t style_weather_screen_calendar_extra_header_main_default;
    ui_init_style(&style_weather_screen_calendar_extra_header_main_default);

    lv_style_set_text_color(&style_weather_screen_calendar_extra_header_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_weather_screen_calendar_extra_header_main_default, &lv_font_puhui_12);
    lv_style_set_text_opa(&style_weather_screen_calendar_extra_header_main_default, 255);
    lv_style_set_bg_opa(&style_weather_screen_calendar_extra_header_main_default, 255);
    lv_style_set_bg_color(&style_weather_screen_calendar_extra_header_main_default, lv_color_hex(0x2195f6));
    lv_style_set_bg_grad_dir(&style_weather_screen_calendar_extra_header_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(weather_screen_calendar_header, &style_weather_screen_calendar_extra_header_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_weather_screen_calendar_main_items_default
    static lv_style_t style_weather_screen_calendar_main_items_default;
    ui_init_style(&style_weather_screen_calendar_main_items_default);

    lv_style_set_bg_opa(&style_weather_screen_calendar_main_items_default, 0);
    lv_style_set_border_width(&style_weather_screen_calendar_main_items_default, 1);
    lv_style_set_border_opa(&style_weather_screen_calendar_main_items_default, 255);
    lv_style_set_border_color(&style_weather_screen_calendar_main_items_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_weather_screen_calendar_main_items_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_weather_screen_calendar_main_items_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_weather_screen_calendar_main_items_default, &lv_font_puhui_12);
    lv_style_set_text_opa(&style_weather_screen_calendar_main_items_default, 255);
    lv_obj_add_style(lv_calendar_get_btnmatrix(ui->weather_screen_calendar), &style_weather_screen_calendar_main_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes weather_screen_cont_3
    ui->weather_screen_cont_3 = lv_obj_create(ui->weather_screen);
    lv_obj_set_pos(ui->weather_screen_cont_3, 365, 225);
    lv_obj_set_size(ui->weather_screen_cont_3, 153, 237);
    lv_obj_set_scrollbar_mode(ui->weather_screen_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for weather_screen_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_cont_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->weather_screen_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->weather_screen_cont_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->weather_screen_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_cont_3, 179, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_cont_3, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_indoor_temp_label
    ui->weather_screen_indoor_temp_label = lv_label_create(ui->weather_screen_cont_3);
    lv_label_set_text(ui->weather_screen_indoor_temp_label, "26℃");
    lv_label_set_long_mode(ui->weather_screen_indoor_temp_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_indoor_temp_label, 36, 19);
    lv_obj_set_size(ui->weather_screen_indoor_temp_label, 71, 32);

    //Write style for weather_screen_indoor_temp_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_indoor_temp_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_indoor_temp_label, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_indoor_temp_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_indoor_temp_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_indoor_temp_label, 155, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_indoor_temp_label, lv_color_hex(0x00d22e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_indoor_temp_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_indoor_temp_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_20
    ui->weather_screen_label_20 = lv_label_create(ui->weather_screen_cont_3);
    lv_label_set_text(ui->weather_screen_label_20, "室内温度");
    lv_label_set_long_mode(ui->weather_screen_label_20, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_20, 37, 65);
    lv_obj_set_size(ui->weather_screen_label_20, 69, 18);

    //Write style for weather_screen_label_20, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_20, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_20, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_20, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_20, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_20, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_indoor_hum_label
    ui->weather_screen_indoor_hum_label = lv_label_create(ui->weather_screen_cont_3);
    lv_label_set_text(ui->weather_screen_indoor_hum_label, "34%");
    lv_label_set_long_mode(ui->weather_screen_indoor_hum_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_indoor_hum_label, 34, 143);
    lv_obj_set_size(ui->weather_screen_indoor_hum_label, 71, 32);

    //Write style for weather_screen_indoor_hum_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_indoor_hum_label, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_indoor_hum_label, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_indoor_hum_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_indoor_hum_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_indoor_hum_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->weather_screen_indoor_hum_label, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->weather_screen_indoor_hum_label, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_indoor_hum_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes weather_screen_label_22
    ui->weather_screen_label_22 = lv_label_create(ui->weather_screen_cont_3);
    lv_label_set_text(ui->weather_screen_label_22, "室内湿度");
    lv_label_set_long_mode(ui->weather_screen_label_22, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->weather_screen_label_22, 37, 194);
    lv_obj_set_size(ui->weather_screen_label_22, 69, 18);

    //Write style for weather_screen_label_22, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->weather_screen_label_22, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->weather_screen_label_22, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->weather_screen_label_22, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->weather_screen_label_22, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->weather_screen_label_22, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of weather_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->weather_screen);

    //Init events for screen.
    events_init_weather_screen(ui);
}
