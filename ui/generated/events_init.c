/*
 * Copyright 2025 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "events_init.h"
#include "lvgl.h"
#include <stdio.h>

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "AppBridge.h"
#include "FontManager.h"
#include "custom.h"

static void main_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED: {

        break;
    }
    case LV_EVENT_SCREEN_UNLOADED: {

        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_LEFT: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.main_screen_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_main_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->main_screen, main_screen_event_handler, LV_EVENT_ALL, ui);
}

static void screen_1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_LEFT: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.screen_1_del,
                                  setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false, true);
            break;
        }
        case LV_DIR_RIGHT: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.main_screen, guider_ui.main_screen_del,
                                  &guider_ui.screen_1_del, setup_scr_main_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0,
                                  false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_1_weather_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.weather_screen, guider_ui.weather_screen_del,
                              &guider_ui.screen_1_del, setup_scr_weather_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0,
                              false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_1_cpu_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.cpu_screen, guider_ui.cpu_screen_del, &guider_ui.screen_1_del,
                              setup_scr_cpu_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_1_setting_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.setting_screen, guider_ui.setting_screen_del,
                              &guider_ui.screen_1_del, setup_scr_setting_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0,
                              false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_1_sensor_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.sensor_screen, guider_ui.sensor_screen_del,
                              &guider_ui.screen_1_del, setup_scr_sensor_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0,
                              false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_1_img_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.img_screen, guider_ui.img_screen_del, &guider_ui.screen_1_del,
                              setup_scr_img_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_1_music_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.music_screen, guider_ui.music_screen_del, &guider_ui.screen_1_del,
                              setup_scr_music_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_1(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_1, screen_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_weather_btn, screen_1_weather_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_cpu_btn, screen_1_cpu_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_setting_btn, screen_1_setting_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_sensor_btn, screen_1_sensor_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_img_btn, screen_1_img_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_1_music_btn, screen_1_music_btn_event_handler, LV_EVENT_ALL, ui);
}

static void screen_2_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_RIGHT: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_2_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_2_video_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.video_screen, guider_ui.video_screen_del, &guider_ui.screen_2_del,
                              setup_scr_video_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_2_camera_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.camera_screen, guider_ui.camera_screen_del,
                              &guider_ui.screen_2_del, setup_scr_camera_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0,
                              false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_2(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_2, screen_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_video_btn, screen_2_video_btn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_2_camera_btn, screen_2_camera_btn_event_handler, LV_EVENT_ALL, ui);
}

static void setting_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del,
                                  &guider_ui.setting_screen_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0,
                                  false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_setting_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->setting_screen, setting_screen_event_handler, LV_EVENT_ALL, ui);
}

static void weather_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED: {
        start_weather_timer(&guider_ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED: {
        stop_weather_timer();
        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del,
                                  &guider_ui.weather_screen_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0,
                                  false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_weather_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->weather_screen, weather_screen_event_handler, LV_EVENT_ALL, ui);
}

static void cpu_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.cpu_screen_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_cpu_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->cpu_screen, cpu_screen_event_handler, LV_EVENT_ALL, ui);
}

static void sensor_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED: {
        start_sensor_timer(&guider_ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED: {
        stop_sensor_timer();
        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.sensor_screen_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void sensor_screen_led_btn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_PRESSED: {
        lv_obj_set_style_border_color(guider_ui.sensor_screen_led_btn, lv_color_hex(0x1f67db), LV_PART_MAIN);
        break;
    }
    case LV_EVENT_LONG_PRESSED: {
        lv_obj_clear_flag(guider_ui.sensor_screen_cont_1, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    default:
        break;
    }
}

void events_init_sensor_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->sensor_screen, sensor_screen_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->sensor_screen_led_btn, sensor_screen_led_btn_event_handler, LV_EVENT_ALL, ui);
}

static void img_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED: {

        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.img_screen_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_img_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->img_screen, img_screen_event_handler, LV_EVENT_ALL, ui);
}

static void music_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        lv_obj_add_flag(guider_ui.music_screen_playlist_cont, LV_OBJ_FLAG_HIDDEN);
        break;
    }
    case LV_EVENT_SCREEN_LOADED: {
        bridge_music_scan_dir("/root/Music");
        start_music_timer(&guider_ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED: {
        stop_music_timer();
        break;
    }
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.music_screen_del,
                                  setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void music_screen_play_imgbtn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        if (lv_obj_has_state(guider_ui.music_screen_play_imgbtn, LV_STATE_CHECKED))
        {
            bridge_music_play();
        }
        else
        {
            bridge_music_pause();
        }
        break;
    }
    default:
        break;
    }
}

static void music_screen_prev_imgbtn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        bridge_music_prev();

        break;
    }
    default:
        break;
    }
}

static void music_screen_next_imgbtn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        bridge_music_next();

        break;
    }
    default:
        break;
    }
}

static void music_screen_off_imgbtn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.music_screen_del,
                              setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
        break;
    }
    default:
        break;
    }
}

static void music_screen_music_list_imgbtn_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        lv_obj_clear_flag(guider_ui.music_screen_playlist_cont, LV_OBJ_FLAG_HIDDEN);
        size_t count;
        char **playlist = bridge_get_music_playlist(&count);
        lv_obj_t *btn;

        if (playlist != NULL)
        {
            lv_obj_clean(guider_ui.music_screen_playlist_list);
            for (size_t i = 0; i < count; i++)
            {
                // 2. 使用字符串，例如添加到列表控件
                btn = lv_list_add_btn(guider_ui.music_screen_playlist_list, LV_SYMBOL_FILE, playlist[i]);
                lv_obj_add_style(btn, &font_style_24, 0);
                printf("Song: %s\n", playlist[i]);
            }

            // 3. 重要：使用完后必须释放内存！
            bridge_free_music_playlist(playlist, count);
        }
        break;
    }
    default:
        break;
    }
}

static void music_screen_sound_slider_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED: {

        break;
    }
    default:
        break;
    }
}

static void music_screen_progress_bar_slider_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_VALUE_CHANGED: {

        break;
    }
    default:
        break;
    }
}

void events_init_music_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->music_screen, music_screen_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_play_imgbtn, music_screen_play_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_prev_imgbtn, music_screen_prev_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_next_imgbtn, music_screen_next_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_off_imgbtn, music_screen_off_imgbtn_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_music_list_imgbtn, music_screen_music_list_imgbtn_event_handler, LV_EVENT_ALL,
                        ui);
    lv_obj_add_event_cb(ui->music_screen_sound_slider, music_screen_sound_slider_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->music_screen_progress_bar_slider, music_screen_progress_bar_slider_event_handler,
                        LV_EVENT_ALL, ui);
}

static void video_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.video_screen_del,
                                  setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_video_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->video_screen, video_screen_event_handler, LV_EVENT_ALL, ui);
}

static void camera_screen_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch (dir)
        {
        case LV_DIR_TOP: {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_2, guider_ui.screen_2_del, &guider_ui.camera_screen_del,
                                  setup_scr_screen_2, LV_SCR_LOAD_ANIM_MOVE_TOP, 300, 0, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void camera_screen_img_1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code)
    {
    case LV_EVENT_CLICKED: {
        ui_load_scr_animation(&guider_ui, &guider_ui.img_screen, guider_ui.img_screen_del, &guider_ui.camera_screen_del,
                              setup_scr_img_screen, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_camera_screen(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->camera_screen, camera_screen_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->camera_screen_img_1, camera_screen_img_1_event_handler, LV_EVENT_ALL, ui);
}

void events_init(lv_ui *ui)
{
    
}
