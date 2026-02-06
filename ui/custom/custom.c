/*
 * Copyright 2023 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

/*********************
 *      INCLUDES
 *********************/
#include "custom.h"
#include "app_bridge.h"
#include "fonts.h"
#include "lvgl.h"
#include "main.h" // 引入 main.h 以使用 font_style_32
#include "qweather_icons.h"
#include <stdio.h>
#include <stdlib.h> // for free
#include <time.h>


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_timer_t *music_timer = NULL;
static lv_timer_t *weather_timer = NULL;
static lv_timer_t *system_time_timer = NULL;
static lv_timer_t *sensor_timer = NULL;

/**
 * @brief 获取当前格式化时间字符串
 * @param buf 缓冲区
 * @param size 缓冲区大小
 * @param format 格式字符串 (例如 "%H:%M")
 */
static void get_local_time_str(char *buf, size_t size, const char *format)
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, size, format, timeinfo);
}

/**
 * Create a demo application
 */
void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void format_time_str(char *buf, size_t len, double seconds)
{
    int total_sec = (int)seconds;
    int m = total_sec / 60;
    int s = total_sec % 60;
    snprintf(buf, len, "%02d:%02d", m, s);
}

void music_update_timer_cb(lv_timer_t *timer)
{
    lv_ui *ui = (lv_ui *)timer->user_data;
    char time_buf[16];

    // 1. 获取并显示当前播放时间
    double cur_time = bridge_music_current_time();
    if (ui->music_screen_process_bar_start_label)
    {
        format_time_str(time_buf, sizeof(time_buf), cur_time);
        lv_label_set_text(ui->music_screen_process_bar_start_label, time_buf);
    }

    // 2. 获取并显示总时长
    double duration = bridge_music_duration();
    if (ui->music_screen_process_bar_end_label)
    {
        format_time_str(time_buf, sizeof(time_buf), duration);
        lv_label_set_text(ui->music_screen_process_bar_end_label, time_buf);
    }

    // 3. 如果有进度条，也需要更新范围和当前值
    if (ui->music_screen_progress_bar_slider)
    {
        if (duration > 0)
        {
            lv_slider_set_range(ui->music_screen_progress_bar_slider, 0, (int32_t)duration);
            lv_slider_set_value(ui->music_screen_progress_bar_slider, (int32_t)cur_time, LV_ANIM_ON);
        }
        else
        {
            lv_slider_set_value(ui->music_screen_progress_bar_slider, 0, LV_ANIM_OFF);
        }
    }

    // 4. 更新歌名
    if (ui->music_screen_song_name_label)
    {
        char *name = bridge_current_music_name();
        if (name)
        {
            // 只有当文本发生变化时才更新，避免不必要的重绘
            if (strcmp(lv_label_get_text(ui->music_screen_song_name_label), name) != 0)
            {
                lv_label_set_text(ui->music_screen_song_name_label, name);

                // 触发专辑图片更新 (仅在切歌时执行一次)
                if (ui->music_screen_album_img)
                {
                    char *cover_path = bridge_get_current_album_cover_path();
                    if (cover_path)
                    {
                        lv_img_set_src(ui->music_screen_album_img, cover_path);
                        free(cover_path);
                    }
                    else
                    {
                        // 如果没有封面，使用默认图片
                        lv_img_set_src(ui->music_screen_album_img, &_bizhi_alpha_320x320);
                    }
                }
            }
            free(name); // 释放 bridge 分配的内存
        }
    }

    // 5. 更新歌词 (滚动显示当前行)
    if (ui->music_screen_song_lyrics_label)
    {
        char *line = bridge_get_current_lyric_line();
        if (line)
        {
            // 只有当文本发生变化时才更新
            if (strcmp(lv_label_get_text(ui->music_screen_song_lyrics_label), line) != 0)
            {
                lv_label_set_text(ui->music_screen_song_lyrics_label, line);
            }
            free(line);
        }
    }

    // 6. 更新专辑图片 (假设 bridge 层有对应接口，或者通过文件名推断)
    // 注意：图片加载比较耗时，通常不建议在 500ms 定时器里频繁做 IO 操作
    // 最好是在切歌时触发一次更新，而不是轮询。
    // 但如果必须在这里写，可以加个静态变量判断是否切歌了。
}

void start_music_timer(lv_ui *ui)
{
    // 应用字体样式 (HP Font, Size 32)
    // 注意：font_style_32 在 main.cpp 中定义并在 main.h 中声明
    if (ui->music_screen_song_name_label)
    {
        // 移除 GUI Guider 生成的本地字体样式，以便 font_style_32 生效
        lv_obj_remove_local_style_prop(ui->music_screen_song_name_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->music_screen_song_name_label, &font_style_32, 0);
    }
    if (ui->music_screen_song_lyrics_label)
    {
        // 移除 GUI Guider 生成的本地字体样式，以便 font_style_32 生效
        lv_obj_remove_local_style_prop(ui->music_screen_song_lyrics_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->music_screen_song_lyrics_label, &font_style_32, 0);
    }

    if (music_timer == NULL)
    {
        music_timer = lv_timer_create(music_update_timer_cb, 500, ui);
    }
}

void stop_music_timer()
{
    if (music_timer != NULL)
    {
        lv_timer_del(music_timer);
        music_timer = NULL;
    }
}

void weather_update_timer_cb(lv_timer_t *timer)
{
    lv_ui *ui = (lv_ui *)timer->user_data;

    // --- Time Update Logic ---
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char time_buf[32];
    char date_buf[32];

    if (ui->weather_screen_time_label)
    {
        strftime(time_buf, sizeof(time_buf), "%H:%M", timeinfo);
        lv_label_set_text(ui->weather_screen_time_label, time_buf);
    }

    if (ui->weather_screen_date_label)
    {
        char date_part[32];
        strftime(date_part, sizeof(date_part), "%Y-%m-%d", timeinfo);
        const char *weekdays[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
        snprintf(date_buf, sizeof(date_buf), "%s %s", date_part, weekdays[timeinfo->tm_wday]);
        lv_label_set_text(ui->weather_screen_date_label, date_buf);
    }

    if (ui->weather_screen_calendar)
    {
        lv_calendar_set_today_date(ui->weather_screen_calendar, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                                   timeinfo->tm_mday);
        lv_calendar_set_showed_date(ui->weather_screen_calendar, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
    }
    // -------------------------

    // 获取最新数据 (注意：这里只获取缓存的数据，不触发网络请求)
    AppWeatherData data;
    bridge_get_weather_data(&data);

    // 1. 城市
    if (ui->weather_screen_location_label)
    {
        // lv_label_set_text(ui->weather_screen_location_label, data.city);
    }

    // 2. 天气状况 (多云/晴)
    if (ui->weather_screen_weather_text_label)
    {
        lv_label_set_text(ui->weather_screen_weather_text_label, data.weather);
    }

    // 3. 温度
    if (ui->weather_screen_temp_label)
    {
        char temp_buf[16];
        snprintf(temp_buf, sizeof(temp_buf), "%d°C", data.temperature);
        lv_label_set_text(ui->weather_screen_temp_label, temp_buf);
    }

    // 5. 风向风速
    if (ui->weather_screen_winddir_label)
    {
        char wind_buf[32];
        snprintf(wind_buf, sizeof(wind_buf), "%s%d级", data.windDir, data.windSpeed);
        lv_label_set_text(ui->weather_screen_winddir_label, wind_buf);
    }

    // 6. 天气图标 (这里假设使用 day1 的图标作为主图标，或者你需要一个单独的主图标 Label)
    if (ui->weather_screen_weather_icon_label)
    {
        lv_label_set_text(ui->weather_screen_weather_icon_label, get_weather_icon_str(data.iconCode));
    }

    // // 7. 预报更新
    if (ui->weather_screen_day1_temp_label)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d℃-%d℃", data.day1_tempMin, data.day1_tempMax);
        lv_label_set_text(ui->weather_screen_day1_temp_label, buf);
    }
    if (ui->weather_screen_day1_winddir_label)
    {
        char wind_buf[32];
        snprintf(wind_buf, sizeof(wind_buf), "%s", data.day1_windDir);
        lv_label_set_text(ui->weather_screen_day1_winddir_label, wind_buf);
    }
    if (ui->weather_screen_day1_weather_icon_label)
    {
        lv_label_set_text(ui->weather_screen_day1_weather_icon_label, get_weather_icon_str(data.iconCode));
    }
    if (ui->weather_screen_day2_temp_label)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d℃-%d℃", data.day2_tempMin, data.day2_tempMax);
        lv_label_set_text(ui->weather_screen_day2_temp_label, buf);
    }
    if (ui->weather_screen_day2_winddir_label)
    {
        char wind_buf[32];
        snprintf(wind_buf, sizeof(wind_buf), "%s", data.day2_windDir);
        lv_label_set_text(ui->weather_screen_day2_winddir_label, wind_buf);
    }
    if (ui->weather_screen_day2_weather_icon_label)
    {
        lv_label_set_text(ui->weather_screen_day2_weather_icon_label, get_weather_icon_str(data.day2_iconCode));
    }
    if (ui->weather_screen_day3_temp_label)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d℃-%d℃", data.day3_tempMin, data.day3_tempMax);
        lv_label_set_text(ui->weather_screen_day3_temp_label, buf);
    }
    if (ui->weather_screen_day3_winddir_label)
    {
        char wind_buf[32];
        snprintf(wind_buf, sizeof(wind_buf), "%s", data.day3_windDir);
        lv_label_set_text(ui->weather_screen_day3_winddir_label, wind_buf);
    }
    if (ui->weather_screen_day3_weather_icon_label)
    {
        lv_label_set_text(ui->weather_screen_day3_weather_icon_label, get_weather_icon_str(data.day3_iconCode));
    }
}

void start_weather_timer(lv_ui *ui)
{
    // 1. 【关键修复】解决字体被裁剪/移位问题：强制设置为自适应大小
    if (ui->weather_screen_temp_label)
        lv_obj_set_size(ui->weather_screen_temp_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if (ui->weather_screen_location_label)
        lv_obj_set_size(ui->weather_screen_location_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if (ui->weather_screen_weather_text_label)
        lv_obj_set_size(ui->weather_screen_weather_text_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if (ui->weather_screen_winddir_label)
        lv_obj_set_size(ui->weather_screen_winddir_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if (ui->weather_screen_weather_icon_label)
        lv_obj_set_size(ui->weather_screen_weather_icon_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if (ui->weather_screen_date_label)
        lv_obj_set_size(ui->weather_screen_date_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // 预报部分
    // Day 1
    if (ui->weather_screen_day1_temp_label)
    {
        lv_obj_set_width(ui->weather_screen_day1_temp_label, 90);
        lv_obj_set_height(ui->weather_screen_day1_temp_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day1_winddir_label)
    {
        lv_obj_set_width(ui->weather_screen_day1_winddir_label, 96);
        lv_obj_set_height(ui->weather_screen_day1_winddir_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day1_weather_icon_label)
    {
        lv_obj_set_width(ui->weather_screen_day1_weather_icon_label, 32);
        lv_obj_set_height(ui->weather_screen_day1_weather_icon_label, LV_SIZE_CONTENT);
    }

    // Day 2
    if (ui->weather_screen_day2_temp_label)
    {
        lv_obj_set_width(ui->weather_screen_day2_temp_label, 90);
        lv_obj_set_height(ui->weather_screen_day2_temp_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day2_winddir_label)
    {
        lv_obj_set_width(ui->weather_screen_day2_winddir_label, 96);
        lv_obj_set_height(ui->weather_screen_day2_winddir_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day2_weather_icon_label)
    {
        lv_obj_set_width(ui->weather_screen_day2_weather_icon_label, 32);
        lv_obj_set_height(ui->weather_screen_day2_weather_icon_label, LV_SIZE_CONTENT);
    }

    // Day 3
    if (ui->weather_screen_day3_temp_label)
    {
        lv_obj_set_width(ui->weather_screen_day3_temp_label, 90);
        lv_obj_set_height(ui->weather_screen_day3_temp_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day3_winddir_label)
    {
        lv_obj_set_width(ui->weather_screen_day3_winddir_label, 96);
        lv_obj_set_height(ui->weather_screen_day3_winddir_label, LV_SIZE_CONTENT);
    }
    if (ui->weather_screen_day3_weather_icon_label)
    {
        lv_obj_set_width(ui->weather_screen_day3_weather_icon_label, 32);
        lv_obj_set_height(ui->weather_screen_day3_weather_icon_label, LV_SIZE_CONTENT);
    }

    // 每次进入屏幕时触发一次网络更新 (异步)
    bridge_update_weather();

    // 应用字体样式 (HP Font, Size 24)
    if (ui->weather_screen_temp_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_temp_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_temp_label, &font_style_24, 0);
    }
    if (ui->weather_screen_winddir_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_winddir_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_winddir_label, &font_style_24, 0);
    }
    if (ui->weather_screen_weather_text_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_weather_text_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_weather_text_label, &font_style_24, 0);
    }
    if (ui->weather_screen_date_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_date_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_date_label, &font_style_24, 0);
    }
    if (ui->weather_screen_weather_icon_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_weather_icon_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_weather_icon_label, &weather_font_style_128, 0);
        // 调整图标位置,解决freetype带来的偏差
        lv_obj_set_y(ui->weather_screen_weather_icon_label, lv_obj_get_y(ui->weather_screen_weather_icon_label) - 25);
        // lv_obj_set_style_bg_color(ui->weather_screen_weather_icon_label, lv_palette_main(LV_PALETTE_RED), 0);
        // lv_obj_set_style_bg_opa(ui->weather_screen_weather_icon_label, LV_OPA_50, 0); //调试
    }
    if (ui->weather_screen_day1_temp_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day1_temp_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day1_temp_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day1_winddir_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day1_winddir_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day1_winddir_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day1_weather_icon_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day1_weather_icon_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day1_weather_icon_label, &weather_font_style_32, 0);
        // 调整图标位置,解决freetype带来的偏差
        lv_obj_set_y(ui->weather_screen_day1_weather_icon_label,
                     lv_obj_get_y(ui->weather_screen_day1_weather_icon_label) - 5);
        // lv_obj_set_style_bg_color(ui->weather_screen_weather_icon_label, lv_palette_main(LV_PALETTE_RED), 0);
        // lv_obj_set_style_bg_opa(ui->weather_screen_weather_icon_label, LV_OPA_50, 0); //调试
    }
    if (ui->weather_screen_day2_temp_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day2_temp_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day2_temp_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day2_winddir_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day2_winddir_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day2_winddir_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day2_weather_icon_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day2_weather_icon_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day2_weather_icon_label, &weather_font_style_32, 0);
        // 调整图标位置,解决freetype带来的偏差
        lv_obj_set_y(ui->weather_screen_day2_weather_icon_label,
                     lv_obj_get_y(ui->weather_screen_day2_weather_icon_label) - 5);
        // lv_obj_set_style_bg_color(ui->weather_screen_weather_icon_label, lv_palette_main(LV_PALETTE_RED), 0);
        // lv_obj_set_style_bg_opa(ui->weather_screen_weather_icon_label, LV_OPA_50, 0); //调试
    }
    if (ui->weather_screen_day3_temp_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day3_temp_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day3_temp_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day3_winddir_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day3_winddir_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day3_winddir_label, &font_style_16, 0);
    }
    if (ui->weather_screen_day3_weather_icon_label != NULL)
    {
        lv_obj_remove_local_style_prop(ui->weather_screen_day3_weather_icon_label, LV_STYLE_TEXT_FONT,
                                       LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_style(ui->weather_screen_day3_weather_icon_label, &weather_font_style_32, 0);
        // 调整图标位置,解决freetype带来的偏差
        lv_obj_set_y(ui->weather_screen_day3_weather_icon_label,
                     lv_obj_get_y(ui->weather_screen_day3_weather_icon_label) - 5);
        // lv_obj_set_style_bg_color(ui->weather_screen_weather_icon_label, lv_palette_main(LV_PALETTE_RED), 0);
        // lv_obj_set_style_bg_opa(ui->weather_screen_weather_icon_label, LV_OPA_50, 0); //调试
    }

    if (weather_timer == NULL)
    {
        // 1秒刷新一次 UI，检查数据是否有更新
        weather_timer = lv_timer_create(weather_update_timer_cb, 1000, ui);

        // 立即执行一次 UI 更新 (显示缓存数据)
        weather_update_timer_cb(weather_timer);
    }
}

void stop_weather_timer()
{
    if (weather_timer != NULL)
    {
        lv_timer_del(weather_timer);
        weather_timer = NULL;
    }
}

void sensor_update_timer_cb(lv_timer_t *timer)
{
    lv_ui *ui = (lv_ui *)timer->user_data;

    // 读取温湿度传感器数据
    int temperature = bridge_get_temp();
    int humidity = bridge_get_humi();


    // 更新 UI 显示
    if (ui->sensor_screen_label_8)
    {
        char temp_buf[16];
        snprintf(temp_buf, sizeof(temp_buf), "温度: %d℃", temperature);
        lv_label_set_text(ui->sensor_screen_label_8, temp_buf);
    }

    if (ui->sensor_screen_label_9)
    {
        char hum_buf[16];
        snprintf(hum_buf, sizeof(hum_buf), "湿度: %d%%", humidity);
        lv_label_set_text(ui->sensor_screen_label_9, hum_buf);
    }
}

void start_sensor_timer(lv_ui *ui)
{
    if (sensor_timer == NULL)
    {
        sensor_timer = lv_timer_create(sensor_update_timer_cb, 2000, ui);
    }
}

void stop_sensor_timer()
{
    if (sensor_timer != NULL)
    {
        lv_timer_del(sensor_timer);
        sensor_timer = NULL;
    }
}

