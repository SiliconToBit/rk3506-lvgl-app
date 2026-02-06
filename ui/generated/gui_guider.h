/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *main_screen;
	bool main_screen_del;
	lv_obj_t *main_screen_img_1;
	lv_obj_t *main_screen_label_1;
	lv_obj_t *screen_1;
	bool screen_1_del;
	lv_obj_t *screen_1_weather_btn;
	lv_obj_t *screen_1_weather_btn_label;
	lv_obj_t *screen_1_cpu_btn;
	lv_obj_t *screen_1_cpu_btn_label;
	lv_obj_t *screen_1_setting_btn;
	lv_obj_t *screen_1_setting_btn_label;
	lv_obj_t *screen_1_sensor_btn;
	lv_obj_t *screen_1_sensor_btn_label;
	lv_obj_t *screen_1_img_btn;
	lv_obj_t *screen_1_img_btn_label;
	lv_obj_t *screen_1_music_btn;
	lv_obj_t *screen_1_music_btn_label;
	lv_obj_t *screen_2;
	bool screen_2_del;
	lv_obj_t *screen_2_video_btn;
	lv_obj_t *screen_2_video_btn_label;
	lv_obj_t *screen_2_camera_btn;
	lv_obj_t *screen_2_camera_btn_label;
	lv_obj_t *setting_screen;
	bool setting_screen_del;
	lv_obj_t *setting_screen_label_3;
	lv_obj_t *setting_screen_slider_3;
	lv_obj_t *setting_screen_label_4;
	lv_obj_t *setting_screen_label_5;
	lv_obj_t *setting_screen_slider_4;
	lv_obj_t *setting_screen_label_6;
	lv_obj_t *setting_screen_WIFI;
	lv_obj_t *setting_screen_sw_2;
	lv_obj_t *setting_screen_list_3;
	lv_obj_t *setting_screen_list_3_item0;
	lv_obj_t *setting_screen_cont_1;
	lv_obj_t *setting_screen_ta_1;
	lv_obj_t *setting_screen_btn_1;
	lv_obj_t *setting_screen_btn_1_label;
	lv_obj_t *setting_screen_label_7;
	lv_obj_t *setting_screen_label_8;
	lv_obj_t *weather_screen;
	bool weather_screen_del;
	lv_obj_t *weather_screen_cont_1;
	lv_obj_t *weather_screen_time_label;
	lv_obj_t *weather_screen_date_label;
	lv_obj_t *weather_screen_location_label;
	lv_obj_t *weather_screen_weather_text_label;
	lv_obj_t *weather_screen_img_1;
	lv_obj_t *weather_screen_winddir_label;
	lv_obj_t *weather_screen_temp_label;
	lv_obj_t *weather_screen_weather_icon_label;
	lv_obj_t *weather_screen_cont_2;
	lv_obj_t *weather_screen_day3_temp_label;
	lv_obj_t *weather_screen_label_5;
	lv_obj_t *weather_screen_line_1;
	lv_obj_t *weather_screen_label_6;
	lv_obj_t *weather_screen_label_7;
	lv_obj_t *weather_screen_label_8;
	lv_obj_t *weather_screen_day1_temp_label;
	lv_obj_t *weather_screen_day2_temp_label;
	lv_obj_t *weather_screen_day1_winddir_label;
	lv_obj_t *weather_screen_day2_winddir_label;
	lv_obj_t *weather_screen_day3_winddir_label;
	lv_obj_t *weather_screen_day1_weather_icon_label;
	lv_obj_t *weather_screen_day2_weather_icon_label;
	lv_obj_t *weather_screen_day3_weather_icon_label;
	lv_obj_t *weather_screen_calendar;
	lv_obj_t *weather_screen_cont_3;
	lv_obj_t *weather_screen_indoor_temp_label;
	lv_obj_t *weather_screen_label_20;
	lv_obj_t *weather_screen_indoor_hum_label;
	lv_obj_t *weather_screen_label_22;
	lv_obj_t *cpu_screen;
	bool cpu_screen_del;
	lv_obj_t *sensor_screen;
	bool sensor_screen_del;
	lv_obj_t *sensor_screen_led_btn;
	lv_obj_t *sensor_screen_led_btn_label;
	lv_obj_t *sensor_screen_label_1;
	lv_obj_t *sensor_screen_cont_1;
	lv_obj_t *sensor_screen_img_1;
	lv_obj_t *sensor_screen_img_2;
	lv_obj_t *sensor_screen_btn_1;
	lv_obj_t *sensor_screen_btn_1_label;
	lv_obj_t *sensor_screen_label_2;
	lv_obj_t *sensor_screen_cont_2;
	lv_obj_t *sensor_screen_cont_3;
	lv_obj_t *sensor_screen_img_3;
	lv_obj_t *sensor_screen_label_3;
	lv_obj_t *sensor_screen_label_4;
	lv_obj_t *sensor_screen_label_5;
	lv_obj_t *sensor_screen_label_6;
	lv_obj_t *sensor_screen_label_7;
	lv_obj_t *sensor_screen_img_4;
	lv_obj_t *sensor_screen_label_8;
	lv_obj_t *sensor_screen_label_9;
	lv_obj_t *img_screen;
	bool img_screen_del;
	lv_obj_t *music_screen;
	bool music_screen_del;
	lv_obj_t *music_screen_album_img;
	lv_obj_t *music_screen_song_name_label;
	lv_obj_t *music_screen_song_lyrics_label;
	lv_obj_t *music_screen_play_imgbtn;
	lv_obj_t *music_screen_play_imgbtn_label;
	lv_obj_t *music_screen_prev_imgbtn;
	lv_obj_t *music_screen_prev_imgbtn_label;
	lv_obj_t *music_screen_next_imgbtn;
	lv_obj_t *music_screen_next_imgbtn_label;
	lv_obj_t *music_screen_off_imgbtn;
	lv_obj_t *music_screen_off_imgbtn_label;
	lv_obj_t *music_screen_sound_imgbtn;
	lv_obj_t *music_screen_sound_imgbtn_label;
	lv_obj_t *music_screen_music_list_imgbtn;
	lv_obj_t *music_screen_music_list_imgbtn_label;
	lv_obj_t *music_screen_sound_slider;
	lv_obj_t *music_screen_progress_bar_slider;
	lv_obj_t *music_screen_process_bar_start_label;
	lv_obj_t *music_screen_process_bar_end_label;
	lv_obj_t *music_screen_playlist_cont;
	lv_obj_t *music_screen_playlist_list;
	lv_obj_t *music_screen_playlist_list_item0;
	lv_obj_t *music_screen_label_1;
	lv_obj_t *video_screen;
	bool video_screen_del;
	lv_obj_t *camera_screen;
	bool camera_screen_del;
	lv_obj_t *camera_screen_img_3;
	lv_obj_t *camera_screen_img_2;
	lv_obj_t *camera_screen_img_1;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_main_screen(lv_ui *ui);
void setup_scr_screen_1(lv_ui *ui);
void setup_scr_screen_2(lv_ui *ui);
void setup_scr_setting_screen(lv_ui *ui);
void setup_scr_weather_screen(lv_ui *ui);
void setup_scr_cpu_screen(lv_ui *ui);
void setup_scr_sensor_screen(lv_ui *ui);
void setup_scr_img_screen(lv_ui *ui);
void setup_scr_music_screen(lv_ui *ui);
void setup_scr_video_screen(lv_ui *ui);
void setup_scr_camera_screen(lv_ui *ui);
LV_IMG_DECLARE(_bizhi_alpha_854x480);

LV_IMG_DECLARE(_weather_120x120);

LV_IMG_DECLARE(_cpu_120x120);

LV_IMG_DECLARE(_setting_120x120);

LV_IMG_DECLARE(_sensor_120x120);

LV_IMG_DECLARE(_img_120x120);

LV_IMG_DECLARE(_music_120x120);

LV_IMG_DECLARE(_video_120x120);

LV_IMG_DECLARE(_camera_120x120);
LV_IMG_DECLARE(_location_alpha_50x50);
LV_IMG_DECLARE(_led_alpha_80x80);
LV_IMG_DECLARE(_alarm_alpha_80x80);
LV_IMG_DECLARE(_CPU2_alpha_100x100);
LV_IMG_DECLARE(_temp_alpha_80x80);
LV_IMG_DECLARE(_bizhi_alpha_320x320);
LV_IMG_DECLARE(_play_alpha_70x70);
LV_IMG_DECLARE(_pause_alpha_70x70);
LV_IMG_DECLARE(_pre_alpha_70x70);
LV_IMG_DECLARE(_next_alpha_70x70);
LV_IMG_DECLARE(_off_alpha_70x70);
LV_IMG_DECLARE(_sound_alpha_50x50);
LV_IMG_DECLARE(_list2_alpha_70x70);
LV_IMG_DECLARE(_photograph_alpha_100x100);
LV_IMG_DECLARE(_img_alpha_100x100);

LV_FONT_DECLARE(lv_font_puhui_64)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_puhui_32)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_puhui_24)
LV_FONT_DECLARE(lv_font_puhui_16)
LV_FONT_DECLARE(lv_font_qweather_icons_128)
LV_FONT_DECLARE(lv_font_qweather_icons_32)
LV_FONT_DECLARE(lv_font_puhui_12)
LV_FONT_DECLARE(lv_font_puhui_18)


#ifdef __cplusplus
}
#endif
#endif
