#ifndef APP_BRIDGE_H
#define APP_BRIDGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "string.h"

    // 供 UI 事件调用的 C 接口
    void bridge_music_scan_dir(const char *path);
    char **bridge_get_music_playlist(size_t *out_count);
    void bridge_free_music_playlist(char **playlist, size_t count);
    void bridge_music_play(void);
    void bridge_music_pause(void);
    void bridge_music_prev(void);
    void bridge_music_next(void);
    void bridge_update_weather(void);
    void bridge_update_weather_city(const char *city);

    // 天气数据结构 (C 兼容)
    typedef struct
    {
        char city[64];
        char weather[64];
        int iconCode;
        int temperature;
        int humidity;
        int feelsLike;
        char windDir[64];
        int windSpeed;

        // 预报 Day 1 (今天)
        int day1_iconCode;
        int day1_tempMin;
        int day1_tempMax;
        char day1_windDir[64];
        char day1_fxDate[64];

        // 预报 Day 2 (明天)
        int day2_iconCode;
        int day2_tempMin;
        int day2_tempMax;
        char day2_windDir[64];

        // 预报 Day 3 (后天)
        int day3_iconCode;
        int day3_tempMin;
        int day3_tempMax;
        char day3_windDir[64];
    } AppWeatherData;

    // 获取完整天气数据
    void bridge_get_weather_data(AppWeatherData *data);

    double bridge_music_current_time(void);
    double bridge_music_duration(void);
    char *bridge_current_music_name();
    char *bridge_current_song_lyrics(void);
    char *bridge_get_current_lyric_line(void);
    char *bridge_get_current_album_cover_path(void);
    void bridge_set_volume(long volume);

    // 获取传感器数据的接口
    int bridge_get_temp(void);
    int bridge_get_humi(void);

    void bridge_set_brightness(int level);
    int bridge_get_brightness(void);

    //

#ifdef __cplusplus
}
#endif

#endif