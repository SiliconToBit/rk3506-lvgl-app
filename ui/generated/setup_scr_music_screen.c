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



void setup_scr_music_screen(lv_ui *ui)
{
    //Write codes music_screen
    ui->music_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->music_screen, 854, 480);
    lv_obj_set_scrollbar_mode(ui->music_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for music_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen, 141, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen, lv_color_hex(0x2F92DA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_album_img
    ui->music_screen_album_img = lv_img_create(ui->music_screen);
    lv_obj_add_flag(ui->music_screen_album_img, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->music_screen_album_img, &_bizhi_alpha_320x320);
    lv_img_set_pivot(ui->music_screen_album_img, 50,50);
    lv_img_set_angle(ui->music_screen_album_img, 0);
    lv_obj_set_pos(ui->music_screen_album_img, 27, 77);
    lv_obj_set_size(ui->music_screen_album_img, 320, 320);

    //Write style for music_screen_album_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_album_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->music_screen_album_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_album_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_album_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_song_name_label
    ui->music_screen_song_name_label = lv_label_create(ui->music_screen);
    lv_label_set_text(ui->music_screen_song_name_label, "歌曲名字");
    lv_label_set_long_mode(ui->music_screen_song_name_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_pos(ui->music_screen_song_name_label, 539, 44);
    lv_obj_set_size(ui->music_screen_song_name_label, 192, 40);

    //Write style for music_screen_song_name_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_screen_song_name_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_song_name_label, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_song_name_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_song_name_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_song_name_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_song_lyrics_label
    ui->music_screen_song_lyrics_label = lv_label_create(ui->music_screen);
    lv_label_set_text(ui->music_screen_song_lyrics_label, "好几个好几个就会感觉\n");
    lv_label_set_long_mode(ui->music_screen_song_lyrics_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->music_screen_song_lyrics_label, 501, 131);
    lv_obj_set_size(ui->music_screen_song_lyrics_label, 273, 85);

    //Write style for music_screen_song_lyrics_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_screen_song_lyrics_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_song_lyrics_label, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_song_lyrics_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_song_lyrics_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_song_lyrics_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_play_imgbtn
    ui->music_screen_play_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_obj_add_flag(ui->music_screen_play_imgbtn, LV_OBJ_FLAG_CHECKABLE);
    lv_imgbtn_set_src(ui->music_screen_play_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_play_alpha_70x70, NULL);
    lv_imgbtn_set_src(ui->music_screen_play_imgbtn, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_pause_alpha_70x70, NULL);
    ui->music_screen_play_imgbtn_label = lv_label_create(ui->music_screen_play_imgbtn);
    lv_label_set_text(ui->music_screen_play_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_play_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_play_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_play_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_play_imgbtn, 570, 351);
    lv_obj_set_size(ui->music_screen_play_imgbtn, 70, 70);

    //Write style for music_screen_play_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_play_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_play_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_play_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_play_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_play_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_play_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_play_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_play_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_play_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_play_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_play_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_play_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_play_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_prev_imgbtn
    ui->music_screen_prev_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_imgbtn_set_src(ui->music_screen_prev_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_pre_alpha_70x70, NULL);
    ui->music_screen_prev_imgbtn_label = lv_label_create(ui->music_screen_prev_imgbtn);
    lv_label_set_text(ui->music_screen_prev_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_prev_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_prev_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_prev_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_prev_imgbtn, 464, 351);
    lv_obj_set_size(ui->music_screen_prev_imgbtn, 70, 70);

    //Write style for music_screen_prev_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_prev_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_prev_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_prev_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_prev_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_prev_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_prev_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_prev_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_prev_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_prev_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_prev_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_prev_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_prev_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_prev_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_next_imgbtn
    ui->music_screen_next_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_imgbtn_set_src(ui->music_screen_next_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_next_alpha_70x70, NULL);
    ui->music_screen_next_imgbtn_label = lv_label_create(ui->music_screen_next_imgbtn);
    lv_label_set_text(ui->music_screen_next_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_next_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_next_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_next_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_next_imgbtn, 675, 351);
    lv_obj_set_size(ui->music_screen_next_imgbtn, 70, 70);

    //Write style for music_screen_next_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_next_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_next_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_next_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_next_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_next_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_next_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_next_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_next_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_next_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_next_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_next_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_next_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_next_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_off_imgbtn
    ui->music_screen_off_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_imgbtn_set_src(ui->music_screen_off_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_off_alpha_70x70, NULL);
    ui->music_screen_off_imgbtn_label = lv_label_create(ui->music_screen_off_imgbtn);
    lv_label_set_text(ui->music_screen_off_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_off_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_off_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_off_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_off_imgbtn, 365, 351);
    lv_obj_set_size(ui->music_screen_off_imgbtn, 70, 70);

    //Write style for music_screen_off_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_off_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_off_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_off_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_off_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_off_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_off_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_off_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_off_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_off_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_off_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_off_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_off_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_off_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_sound_imgbtn
    ui->music_screen_sound_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_imgbtn_set_src(ui->music_screen_sound_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_sound_alpha_50x50, NULL);
    ui->music_screen_sound_imgbtn_label = lv_label_create(ui->music_screen_sound_imgbtn);
    lv_label_set_text(ui->music_screen_sound_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_sound_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_sound_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_sound_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_sound_imgbtn, 376, 264);
    lv_obj_set_size(ui->music_screen_sound_imgbtn, 50, 50);

    //Write style for music_screen_sound_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_sound_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_sound_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_sound_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_sound_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_sound_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_sound_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_sound_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_sound_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_sound_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_sound_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_sound_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_sound_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_sound_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_music_list_imgbtn
    ui->music_screen_music_list_imgbtn = lv_imgbtn_create(ui->music_screen);
    lv_imgbtn_set_src(ui->music_screen_music_list_imgbtn, LV_IMGBTN_STATE_RELEASED, NULL, &_list2_alpha_70x70, NULL);
    ui->music_screen_music_list_imgbtn_label = lv_label_create(ui->music_screen_music_list_imgbtn);
    lv_label_set_text(ui->music_screen_music_list_imgbtn_label, "");
    lv_label_set_long_mode(ui->music_screen_music_list_imgbtn_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->music_screen_music_list_imgbtn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->music_screen_music_list_imgbtn, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->music_screen_music_list_imgbtn, 769, 351);
    lv_obj_set_size(ui->music_screen_music_list_imgbtn, 70, 70);

    //Write style for music_screen_music_list_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->music_screen_music_list_imgbtn, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_music_list_imgbtn, &lv_font_puhui_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_music_list_imgbtn, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->music_screen_music_list_imgbtn, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_music_list_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_img_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->music_screen_music_list_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->music_screen_music_list_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for music_screen_music_list_imgbtn, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_img_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->music_screen_music_list_imgbtn, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->music_screen_music_list_imgbtn, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for music_screen_music_list_imgbtn, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
    lv_obj_set_style_img_recolor_opa(ui->music_screen_music_list_imgbtn, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
    lv_obj_set_style_img_opa(ui->music_screen_music_list_imgbtn, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

    //Write codes music_screen_sound_slider
    ui->music_screen_sound_slider = lv_slider_create(ui->music_screen);
    lv_slider_set_range(ui->music_screen_sound_slider, 0, 100);
    lv_slider_set_mode(ui->music_screen_sound_slider, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->music_screen_sound_slider, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->music_screen_sound_slider, 441, 280);
    lv_obj_set_size(ui->music_screen_sound_slider, 178, 15);

    //Write style for music_screen_sound_slider, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_sound_slider, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_sound_slider, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_sound_slider, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_sound_slider, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->music_screen_sound_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_sound_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_sound_slider, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_sound_slider, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_sound_slider, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_sound_slider, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_sound_slider, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for music_screen_sound_slider, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_sound_slider, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_sound_slider, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_sound_slider, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_sound_slider, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes music_screen_progress_bar_slider
    ui->music_screen_progress_bar_slider = lv_slider_create(ui->music_screen);
    lv_slider_set_range(ui->music_screen_progress_bar_slider, 0, 100);
    lv_slider_set_mode(ui->music_screen_progress_bar_slider, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->music_screen_progress_bar_slider, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->music_screen_progress_bar_slider, 399, 455);
    lv_obj_set_size(ui->music_screen_progress_bar_slider, 403, 14);

    //Write style for music_screen_progress_bar_slider, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_progress_bar_slider, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_progress_bar_slider, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_progress_bar_slider, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_progress_bar_slider, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->music_screen_progress_bar_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_progress_bar_slider, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for music_screen_progress_bar_slider, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_progress_bar_slider, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_progress_bar_slider, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_progress_bar_slider, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_progress_bar_slider, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for music_screen_progress_bar_slider, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->music_screen_progress_bar_slider, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_progress_bar_slider, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_progress_bar_slider, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_progress_bar_slider, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes music_screen_process_bar_start_label
    ui->music_screen_process_bar_start_label = lv_label_create(ui->music_screen);
    lv_label_set_text(ui->music_screen_process_bar_start_label, "00:00");
    lv_label_set_long_mode(ui->music_screen_process_bar_start_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->music_screen_process_bar_start_label, 403, 429);
    lv_obj_set_size(ui->music_screen_process_bar_start_label, 61, 16);

    //Write style for music_screen_process_bar_start_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_screen_process_bar_start_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_process_bar_start_label, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_process_bar_start_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_process_bar_start_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_process_bar_start_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_process_bar_end_label
    ui->music_screen_process_bar_end_label = lv_label_create(ui->music_screen);
    lv_label_set_text(ui->music_screen_process_bar_end_label, "04:11");
    lv_label_set_long_mode(ui->music_screen_process_bar_end_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->music_screen_process_bar_end_label, 732, 429);
    lv_obj_set_size(ui->music_screen_process_bar_end_label, 65, 16);

    //Write style for music_screen_process_bar_end_label, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_screen_process_bar_end_label, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_process_bar_end_label, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_process_bar_end_label, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_process_bar_end_label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_process_bar_end_label, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_playlist_cont
    ui->music_screen_playlist_cont = lv_obj_create(ui->music_screen);
    lv_obj_set_pos(ui->music_screen_playlist_cont, 566, 33);
    lv_obj_set_size(ui->music_screen_playlist_cont, 280, 400);
    lv_obj_set_scrollbar_mode(ui->music_screen_playlist_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(ui->music_screen_playlist_cont, LV_OBJ_FLAG_HIDDEN);

    //Write style for music_screen_playlist_cont, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_playlist_cont, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->music_screen_playlist_cont, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->music_screen_playlist_cont, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->music_screen_playlist_cont, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_playlist_cont, 20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_playlist_cont, 181, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->music_screen_playlist_cont, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->music_screen_playlist_cont, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_playlist_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_playlist_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_playlist_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_playlist_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_playlist_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes music_screen_playlist_list
    ui->music_screen_playlist_list = lv_list_create(ui->music_screen_playlist_cont);
    ui->music_screen_playlist_list_item0 = lv_list_add_btn(ui->music_screen_playlist_list, LV_SYMBOL_FILE, "save");
    lv_obj_set_pos(ui->music_screen_playlist_list, 9, 57);
    lv_obj_set_size(ui->music_screen_playlist_list, 261, 344);
    lv_obj_set_scrollbar_mode(ui->music_screen_playlist_list, LV_SCROLLBAR_MODE_OFF);

    //Write style state: LV_STATE_DEFAULT for &style_music_screen_playlist_list_main_main_default
    static lv_style_t style_music_screen_playlist_list_main_main_default;
    ui_init_style(&style_music_screen_playlist_list_main_main_default);

    lv_style_set_pad_top(&style_music_screen_playlist_list_main_main_default, 5);
    lv_style_set_pad_left(&style_music_screen_playlist_list_main_main_default, 5);
    lv_style_set_pad_right(&style_music_screen_playlist_list_main_main_default, 5);
    lv_style_set_pad_bottom(&style_music_screen_playlist_list_main_main_default, 5);
    lv_style_set_bg_opa(&style_music_screen_playlist_list_main_main_default, 180);
    lv_style_set_bg_color(&style_music_screen_playlist_list_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_music_screen_playlist_list_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_music_screen_playlist_list_main_main_default, 1);
    lv_style_set_border_opa(&style_music_screen_playlist_list_main_main_default, 255);
    lv_style_set_border_color(&style_music_screen_playlist_list_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_music_screen_playlist_list_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_music_screen_playlist_list_main_main_default, 20);
    lv_style_set_shadow_width(&style_music_screen_playlist_list_main_main_default, 0);
    lv_obj_add_style(ui->music_screen_playlist_list, &style_music_screen_playlist_list_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_music_screen_playlist_list_main_scrollbar_default
    static lv_style_t style_music_screen_playlist_list_main_scrollbar_default;
    ui_init_style(&style_music_screen_playlist_list_main_scrollbar_default);

    lv_style_set_radius(&style_music_screen_playlist_list_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_music_screen_playlist_list_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_music_screen_playlist_list_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_music_screen_playlist_list_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->music_screen_playlist_list, &style_music_screen_playlist_list_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_music_screen_playlist_list_extra_btns_main_default
    static lv_style_t style_music_screen_playlist_list_extra_btns_main_default;
    ui_init_style(&style_music_screen_playlist_list_extra_btns_main_default);

    lv_style_set_pad_top(&style_music_screen_playlist_list_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_music_screen_playlist_list_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_music_screen_playlist_list_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_music_screen_playlist_list_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_music_screen_playlist_list_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_music_screen_playlist_list_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_music_screen_playlist_list_extra_btns_main_default, &lv_font_puhui_24);
    lv_style_set_text_opa(&style_music_screen_playlist_list_extra_btns_main_default, 255);
    lv_style_set_radius(&style_music_screen_playlist_list_extra_btns_main_default, 20);
    lv_style_set_bg_opa(&style_music_screen_playlist_list_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_music_screen_playlist_list_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_music_screen_playlist_list_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->music_screen_playlist_list_item0, &style_music_screen_playlist_list_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_music_screen_playlist_list_extra_texts_main_default
    static lv_style_t style_music_screen_playlist_list_extra_texts_main_default;
    ui_init_style(&style_music_screen_playlist_list_extra_texts_main_default);

    lv_style_set_pad_top(&style_music_screen_playlist_list_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_music_screen_playlist_list_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_music_screen_playlist_list_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_music_screen_playlist_list_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_music_screen_playlist_list_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_music_screen_playlist_list_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_music_screen_playlist_list_extra_texts_main_default, &lv_font_puhui_12);
    lv_style_set_text_opa(&style_music_screen_playlist_list_extra_texts_main_default, 255);
    lv_style_set_radius(&style_music_screen_playlist_list_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_music_screen_playlist_list_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_music_screen_playlist_list_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_music_screen_playlist_list_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_music_screen_playlist_list_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes music_screen_label_1
    ui->music_screen_label_1 = lv_label_create(ui->music_screen_playlist_cont);
    lv_label_set_text(ui->music_screen_label_1, "歌曲列表");
    lv_label_set_long_mode(ui->music_screen_label_1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->music_screen_label_1, 52, 13);
    lv_obj_set_size(ui->music_screen_label_1, 162, 32);

    //Write style for music_screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->music_screen_label_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->music_screen_label_1, &lv_font_puhui_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->music_screen_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->music_screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->music_screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of music_screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->music_screen);

    //Init events for screen.
    events_init_music_screen(ui);
}
