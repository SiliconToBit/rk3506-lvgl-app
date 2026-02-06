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



void setup_scr_setting_screen(lv_ui *ui)
{
    //Write codes setting_screen
    ui->setting_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->setting_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->setting_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen, 130, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_3
    ui->setting_screen_label_3 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_3, "背光");
    lv_label_set_long_mode(ui->setting_screen_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_screen_label_3, 55, 51);
    lv_obj_set_size(ui->setting_screen_label_3, 64, 32);

    //Write style for setting_screen_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_3, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_slider_3
    ui->setting_screen_slider_3 = lv_slider_create(ui->setting_screen);
    lv_slider_set_range(ui->setting_screen_slider_3, 0, 100);
    lv_slider_set_mode(ui->setting_screen_slider_3, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->setting_screen_slider_3, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->setting_screen_slider_3, 131, 60);
    lv_obj_set_size(ui->setting_screen_slider_3, 201, 16);

    //Write style for setting_screen_slider_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_3, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->setting_screen_slider_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_slider_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_screen_slider_3, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_3, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_3, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_3, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_3, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for setting_screen_slider_3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_3, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_3, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_3, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_3, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_4
    ui->setting_screen_label_4 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_4, "50");
    lv_label_set_long_mode(ui->setting_screen_label_4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_screen_label_4, 348, 51);
    lv_obj_set_size(ui->setting_screen_label_4, 64, 32);

    //Write style for setting_screen_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_4, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_5
    ui->setting_screen_label_5 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_5, "声音");
    lv_label_set_long_mode(ui->setting_screen_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_screen_label_5, 55, 110);
    lv_obj_set_size(ui->setting_screen_label_5, 64, 32);

    //Write style for setting_screen_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_5, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_slider_4
    ui->setting_screen_slider_4 = lv_slider_create(ui->setting_screen);
    lv_slider_set_range(ui->setting_screen_slider_4, 0, 100);
    lv_slider_set_mode(ui->setting_screen_slider_4, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->setting_screen_slider_4, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->setting_screen_slider_4, 131, 118);
    lv_obj_set_size(ui->setting_screen_slider_4, 201, 16);

    //Write style for setting_screen_slider_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_4, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_4, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_4, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->setting_screen_slider_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_slider_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_screen_slider_4, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_4, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_4, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_4, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_4, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for setting_screen_slider_4, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_slider_4, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_slider_4, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_slider_4, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_slider_4, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_6
    ui->setting_screen_label_6 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_6, "50");
    lv_label_set_long_mode(ui->setting_screen_label_6, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_screen_label_6, 348, 110);
    lv_obj_set_size(ui->setting_screen_label_6, 64, 32);

    //Write style for setting_screen_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_6, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_WIFI
    ui->setting_screen_WIFI = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_WIFI, "WIFI");
    lv_label_set_long_mode(ui->setting_screen_WIFI, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->setting_screen_WIFI, 55, 166);
    lv_obj_set_size(ui->setting_screen_WIFI, 73, 32);

    //Write style for setting_screen_WIFI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_WIFI, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_WIFI, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_WIFI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_WIFI, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_sw_2
    ui->setting_screen_sw_2 = lv_switch_create(ui->setting_screen);
    lv_obj_set_pos(ui->setting_screen_sw_2, 139, 172);
    lv_obj_set_size(ui->setting_screen_sw_2, 40, 20);

    //Write style for setting_screen_sw_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_sw_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_sw_2, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_sw_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_screen_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_sw_2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_screen_sw_2, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->setting_screen_sw_2, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->setting_screen_sw_2, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_sw_2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->setting_screen_sw_2, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for setting_screen_sw_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_sw_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_sw_2, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_sw_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_screen_sw_2, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_sw_2, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes setting_screen_list_3
    ui->setting_screen_list_3 = lv_list_create(ui->setting_screen);
    ui->setting_screen_list_3_item0 = lv_list_add_btn(ui->setting_screen_list_3, LV_SYMBOL_WIFI, "save");
    lv_obj_set_pos(ui->setting_screen_list_3, 55, 226);
    lv_obj_set_size(ui->setting_screen_list_3, 343, 214);
    lv_obj_set_scrollbar_mode(ui->setting_screen_list_3, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_setting_screen_list_3_main_main_default
    static lv_style_t style_setting_screen_list_3_main_main_default;
    ui_init_style(&style_setting_screen_list_3_main_main_default);

    lv_style_set_pad_top(&style_setting_screen_list_3_main_main_default, 5);
    lv_style_set_pad_left(&style_setting_screen_list_3_main_main_default, 5);
    lv_style_set_pad_right(&style_setting_screen_list_3_main_main_default, 5);
    lv_style_set_pad_bottom(&style_setting_screen_list_3_main_main_default, 5);
    lv_style_set_bg_opa(&style_setting_screen_list_3_main_main_default, 156);
    lv_style_set_bg_color(&style_setting_screen_list_3_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_setting_screen_list_3_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_setting_screen_list_3_main_main_default, 1);
    lv_style_set_border_opa(&style_setting_screen_list_3_main_main_default, 255);
    lv_style_set_border_color(&style_setting_screen_list_3_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_setting_screen_list_3_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_setting_screen_list_3_main_main_default, 3);
    lv_style_set_shadow_width(&style_setting_screen_list_3_main_main_default, 0);
    lv_obj_add_style(ui->setting_screen_list_3, &style_setting_screen_list_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_setting_screen_list_3_main_scrollbar_default
    static lv_style_t style_setting_screen_list_3_main_scrollbar_default;
    ui_init_style(&style_setting_screen_list_3_main_scrollbar_default);

    lv_style_set_radius(&style_setting_screen_list_3_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_setting_screen_list_3_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_setting_screen_list_3_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_setting_screen_list_3_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->setting_screen_list_3, &style_setting_screen_list_3_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_setting_screen_list_3_extra_btns_main_default
    static lv_style_t style_setting_screen_list_3_extra_btns_main_default;
    ui_init_style(&style_setting_screen_list_3_extra_btns_main_default);

    lv_style_set_pad_top(&style_setting_screen_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_setting_screen_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_setting_screen_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_setting_screen_list_3_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_setting_screen_list_3_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_setting_screen_list_3_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_setting_screen_list_3_extra_btns_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_setting_screen_list_3_extra_btns_main_default, 255);
    lv_style_set_radius(&style_setting_screen_list_3_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_setting_screen_list_3_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_setting_screen_list_3_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_setting_screen_list_3_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->setting_screen_list_3_item0, &style_setting_screen_list_3_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_setting_screen_list_3_extra_texts_main_default
    static lv_style_t style_setting_screen_list_3_extra_texts_main_default;
    ui_init_style(&style_setting_screen_list_3_extra_texts_main_default);

    lv_style_set_pad_top(&style_setting_screen_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_setting_screen_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_setting_screen_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_setting_screen_list_3_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_setting_screen_list_3_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_setting_screen_list_3_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_setting_screen_list_3_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_setting_screen_list_3_extra_texts_main_default, 255);
    lv_style_set_radius(&style_setting_screen_list_3_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_setting_screen_list_3_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_setting_screen_list_3_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_setting_screen_list_3_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_setting_screen_list_3_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes setting_screen_cont_1
    ui->setting_screen_cont_1 = lv_obj_create(ui->setting_screen);
    lv_obj_set_pos(ui->setting_screen_cont_1, 496, 51);
    lv_obj_set_size(ui->setting_screen_cont_1, 300, 392);
    lv_obj_set_scrollbar_mode(ui->setting_screen_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for setting_screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_screen_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_cont_1, 130, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_ta_1
    ui->setting_screen_ta_1 = lv_textarea_create(ui->setting_screen);
    lv_textarea_set_text(ui->setting_screen_ta_1, "gfdgdfgfgdf");
    lv_textarea_set_placeholder_text(ui->setting_screen_ta_1, "");
    lv_textarea_set_password_bullet(ui->setting_screen_ta_1, "*");
    lv_textarea_set_password_mode(ui->setting_screen_ta_1, false);
    lv_textarea_set_one_line(ui->setting_screen_ta_1, false);
    lv_textarea_set_accepted_chars(ui->setting_screen_ta_1, "");
    lv_textarea_set_max_length(ui->setting_screen_ta_1, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->setting_screen_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->setting_screen_ta_1, 508, 118);
    lv_obj_set_size(ui->setting_screen_ta_1, 274, 42);

    //Write style for setting_screen_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->setting_screen_ta_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_ta_1, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_ta_1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_ta_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_ta_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_screen_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->setting_screen_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->setting_screen_ta_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->setting_screen_ta_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for setting_screen_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_ta_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_ta_1, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_ta_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes setting_screen_btn_1
    ui->setting_screen_btn_1 = lv_btn_create(ui->setting_screen);
    ui->setting_screen_btn_1_label = lv_label_create(ui->setting_screen_btn_1);
    lv_label_set_text(ui->setting_screen_btn_1_label, "连接");
    lv_label_set_long_mode(ui->setting_screen_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->setting_screen_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->setting_screen_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->setting_screen_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->setting_screen_btn_1, 513, 169);
    lv_obj_set_size(ui->setting_screen_btn_1, 266, 50);

    //Write style for setting_screen_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->setting_screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->setting_screen_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->setting_screen_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->setting_screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_btn_1, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_7
    ui->setting_screen_label_7 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_7, "连接到WIFI ZTE-1819_5G");
    lv_label_set_long_mode(ui->setting_screen_label_7, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->setting_screen_label_7, 526, 73);
    lv_obj_set_size(ui->setting_screen_label_7, 240, 32);

    //Write style for setting_screen_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_7, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes setting_screen_label_8
    ui->setting_screen_label_8 = lv_label_create(ui->setting_screen);
    lv_label_set_text(ui->setting_screen_label_8, "已连接WIFI ZTE-1819 IP:192.168.5.106");
    lv_label_set_long_mode(ui->setting_screen_label_8, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->setting_screen_label_8, 194, 166);
    lv_obj_set_size(ui->setting_screen_label_8, 204, 32);

    //Write style for setting_screen_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->setting_screen_label_8, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->setting_screen_label_8, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->setting_screen_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->setting_screen_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->setting_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of setting_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->setting_screen);

    //Init events for screen.
    events_init_setting_screen(ui);
}
