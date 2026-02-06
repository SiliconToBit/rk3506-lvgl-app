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



void setup_scr_sensor_screen(lv_ui *ui)
{
    //Write codes sensor_screen
    ui->sensor_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->sensor_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->sensor_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for sensor_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->sensor_screen, 129, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_led_btn
    ui->sensor_screen_led_btn = lv_btn_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_led_btn, LV_OBJ_FLAG_CHECKABLE);
    ui->sensor_screen_led_btn_label = lv_label_create(ui->sensor_screen_led_btn);
    lv_label_set_text(ui->sensor_screen_led_btn_label, "");
    lv_label_set_long_mode(ui->sensor_screen_led_btn_label, LV_LABEL_LONG_SCROLL);
    lv_obj_align(ui->sensor_screen_led_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->sensor_screen_led_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->sensor_screen_led_btn_label, LV_PCT(100));
    lv_obj_set_pos(ui->sensor_screen_led_btn, 64, 36);
    lv_obj_set_size(ui->sensor_screen_led_btn, 150, 150);

    //Write style for sensor_screen_led_btn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->sensor_screen_led_btn, 103, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen_led_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_led_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->sensor_screen_led_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_led_btn, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_led_btn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_led_btn, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_led_btn, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_led_btn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_led_btn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for sensor_screen_led_btn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->sensor_screen_led_btn, 104, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->sensor_screen_led_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_led_btn, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->sensor_screen_led_btn, 5, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_opa(ui->sensor_screen_led_btn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_color(ui->sensor_screen_led_btn, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_side(ui->sensor_screen_led_btn, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->sensor_screen_led_btn, 20, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->sensor_screen_led_btn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->sensor_screen_led_btn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->sensor_screen_led_btn, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->sensor_screen_led_btn, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write codes sensor_screen_label_1
    ui->sensor_screen_label_1 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_1, "RGB灯");
    lv_label_set_long_mode(ui->sensor_screen_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_1, 101, 147);
    lv_obj_set_size(ui->sensor_screen_label_1, 73, 18);

    //Write style for sensor_screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_1, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_cont_1
    ui->sensor_screen_cont_1 = lv_obj_create(ui->sensor_screen);
    lv_obj_set_pos(ui->sensor_screen_cont_1, 261, 114);
    lv_obj_set_size(ui->sensor_screen_cont_1, 380, 200);
    lv_obj_set_scrollbar_mode(ui->sensor_screen_cont_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->sensor_screen_cont_1, LV_OBJ_FLAG_HIDDEN);

    //Write style for sensor_screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->sensor_screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->sensor_screen_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->sensor_screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_cont_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_img_1
    ui->sensor_screen_img_1 = lv_img_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->sensor_screen_img_1, &_led_alpha_80x80);
    lv_img_set_pivot(ui->sensor_screen_img_1, 50,50);
    lv_img_set_angle(ui->sensor_screen_img_1, 0);
    lv_obj_set_pos(ui->sensor_screen_img_1, 96, 48);
    lv_obj_set_size(ui->sensor_screen_img_1, 80, 80);

    //Write style for sensor_screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->sensor_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->sensor_screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->sensor_screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_img_2
    ui->sensor_screen_img_2 = lv_img_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->sensor_screen_img_2, &_alarm_alpha_80x80);
    lv_img_set_pivot(ui->sensor_screen_img_2, 50,50);
    lv_img_set_angle(ui->sensor_screen_img_2, 0);
    lv_obj_set_pos(ui->sensor_screen_img_2, 102, 304);
    lv_obj_set_size(ui->sensor_screen_img_2, 80, 80);

    //Write style for sensor_screen_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->sensor_screen_img_2, 133, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->sensor_screen_img_2, lv_color_hex(0xcf2525), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->sensor_screen_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->sensor_screen_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_btn_1
    ui->sensor_screen_btn_1 = lv_btn_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_btn_1, LV_OBJ_FLAG_CHECKABLE);
    ui->sensor_screen_btn_1_label = lv_label_create(ui->sensor_screen_btn_1);
    lv_label_set_text(ui->sensor_screen_btn_1_label, "");
    lv_label_set_long_mode(ui->sensor_screen_btn_1_label, LV_LABEL_LONG_SCROLL);
    lv_obj_align(ui->sensor_screen_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->sensor_screen_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->sensor_screen_btn_1_label, LV_PCT(100));
    lv_obj_set_pos(ui->sensor_screen_btn_1, 64, 286);
    lv_obj_set_size(ui->sensor_screen_btn_1, 150, 150);

    //Write style for sensor_screen_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->sensor_screen_btn_1, 103, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->sensor_screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_btn_1, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_btn_1, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for sensor_screen_btn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->sensor_screen_btn_1, 104, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->sensor_screen_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->sensor_screen_btn_1, 5, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_opa(ui->sensor_screen_btn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_color(ui->sensor_screen_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_border_side(ui->sensor_screen_btn_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_radius(ui->sensor_screen_btn_1, 20, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->sensor_screen_btn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->sensor_screen_btn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->sensor_screen_btn_1, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->sensor_screen_btn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write codes sensor_screen_label_2
    ui->sensor_screen_label_2 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_2, "警报");
    lv_label_set_long_mode(ui->sensor_screen_label_2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_2, 105, 400);
    lv_obj_set_size(ui->sensor_screen_label_2, 73, 18);

    //Write style for sensor_screen_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_2, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_cont_2
    ui->sensor_screen_cont_2 = lv_obj_create(ui->sensor_screen);
    lv_obj_set_pos(ui->sensor_screen_cont_2, 494, 37);
    lv_obj_set_size(ui->sensor_screen_cont_2, 300, 401);
    lv_obj_set_scrollbar_mode(ui->sensor_screen_cont_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for sensor_screen_cont_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_cont_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->sensor_screen_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->sensor_screen_cont_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->sensor_screen_cont_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_cont_2, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_cont_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen_cont_2, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_cont_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_cont_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_cont_3
    ui->sensor_screen_cont_3 = lv_obj_create(ui->sensor_screen_cont_2);
    lv_obj_set_pos(ui->sensor_screen_cont_3, -196, -3);
    lv_obj_set_size(ui->sensor_screen_cont_3, 150, 150);
    lv_obj_set_scrollbar_mode(ui->sensor_screen_cont_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for sensor_screen_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_cont_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->sensor_screen_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->sensor_screen_cont_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->sensor_screen_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_cont_3, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_cont_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->sensor_screen_cont_3, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->sensor_screen_cont_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_img_3
    ui->sensor_screen_img_3 = lv_img_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_img_3, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->sensor_screen_img_3, &_CPU2_alpha_100x100);
    lv_img_set_pivot(ui->sensor_screen_img_3, 50,50);
    lv_img_set_angle(ui->sensor_screen_img_3, 0);
    lv_obj_set_pos(ui->sensor_screen_img_3, 591, 61);
    lv_obj_set_size(ui->sensor_screen_img_3, 100, 100);

    //Write style for sensor_screen_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->sensor_screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->sensor_screen_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->sensor_screen_img_3, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_3
    ui->sensor_screen_label_3 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_3, "MPU6050");
    lv_label_set_long_mode(ui->sensor_screen_label_3, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_3, 557, 190);
    lv_obj_set_size(ui->sensor_screen_label_3, 168, 32);

    //Write style for sensor_screen_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_3, &lv_font_puhui_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_4
    ui->sensor_screen_label_4 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_4, "PITCH:+000.04");
    lv_label_set_long_mode(ui->sensor_screen_label_4, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_4, 574, 247);
    lv_obj_set_size(ui->sensor_screen_label_4, 133, 22);

    //Write style for sensor_screen_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_4, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_5
    ui->sensor_screen_label_5 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_5, "ROLL:+000.04");
    lv_label_set_long_mode(ui->sensor_screen_label_5, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_5, 574, 286);
    lv_obj_set_size(ui->sensor_screen_label_5, 133, 22);

    //Write style for sensor_screen_label_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_5, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_6
    ui->sensor_screen_label_6 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_6, "YAW:+000.04");
    lv_label_set_long_mode(ui->sensor_screen_label_6, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_6, 574, 320);
    lv_obj_set_size(ui->sensor_screen_label_6, 133, 22);

    //Write style for sensor_screen_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_6, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_6, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_7
    ui->sensor_screen_label_7 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_7, "TEMP:+000.04");
    lv_label_set_long_mode(ui->sensor_screen_label_7, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_7, 574, 359);
    lv_obj_set_size(ui->sensor_screen_label_7, 133, 22);

    //Write style for sensor_screen_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_7, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_7, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_img_4
    ui->sensor_screen_img_4 = lv_img_create(ui->sensor_screen);
    lv_obj_add_flag(ui->sensor_screen_img_4, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->sensor_screen_img_4, &_temp_alpha_80x80);
    lv_img_set_pivot(ui->sensor_screen_img_4, 50,50);
    lv_img_set_angle(ui->sensor_screen_img_4, 0);
    lv_obj_set_pos(ui->sensor_screen_img_4, 333, 49);
    lv_obj_set_size(ui->sensor_screen_img_4, 80, 80);

    //Write style for sensor_screen_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->sensor_screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->sensor_screen_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->sensor_screen_img_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_8
    ui->sensor_screen_label_8 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_8, "温度：20℃");
    lv_label_set_long_mode(ui->sensor_screen_label_8, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_8, 325, 133);
    lv_obj_set_size(ui->sensor_screen_label_8, 98, 18);

    //Write style for sensor_screen_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_8, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes sensor_screen_label_9
    ui->sensor_screen_label_9 = lv_label_create(ui->sensor_screen);
    lv_label_set_text(ui->sensor_screen_label_9, "湿度：5%");
    lv_label_set_long_mode(ui->sensor_screen_label_9, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->sensor_screen_label_9, 325, 161);
    lv_obj_set_size(ui->sensor_screen_label_9, 87, 18);

    //Write style for sensor_screen_label_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->sensor_screen_label_9, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->sensor_screen_label_9, &lv_font_puhui_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->sensor_screen_label_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->sensor_screen_label_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->sensor_screen_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of sensor_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->sensor_screen);

    //Init events for screen.
    events_init_sensor_screen(ui);
}
