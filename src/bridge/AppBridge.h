#ifndef LVGL_APP_BRIDGE_APP_BRIDGE_H
#define LVGL_APP_BRIDGE_APP_BRIDGE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int bridge_init(void);
    void bridge_deinit(void);

    void bridge_music_scan_dir(const char *path);
    char **bridge_get_music_playlist(size_t *out_count);
    void bridge_free_music_playlist(char **playlist, size_t count);
    void bridge_music_play(void);
    void bridge_music_pause(void);
    void bridge_music_prev(void);
    void bridge_music_next(void);
    void bridge_update_weather(void);
    void bridge_update_weather_city(const char *city);

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

        int day1_iconCode;
        int day1_tempMin;
        int day1_tempMax;
        char day1_windDir[64];
        char day1_fxDate[64];

        int day2_iconCode;
        int day2_tempMin;
        int day2_tempMax;
        char day2_windDir[64];

        int day3_iconCode;
        int day3_tempMin;
        int day3_tempMax;
        char day3_windDir[64];
    } AppWeatherData;

    void bridge_get_weather_data(AppWeatherData *data);

    double bridge_music_current_time(void);
    double bridge_music_duration(void);
    char *bridge_current_music_name(void);
    char *bridge_current_song_lyrics(void);
    char *bridge_get_current_lyric_line(void);
    char *bridge_get_current_album_cover_path(void);
    void bridge_set_volume(long volume);

    int bridge_get_temp(void);
    int bridge_get_humi(void);

    void bridge_set_brightness(int level);
    int bridge_get_brightness(void);

    // 设备控制接口
    int bridge_device_init(void);
    void bridge_device_deinit(void);

    // LED 控制
    int bridge_led_add(const char *deviceId, const char *gpioPath);
    int bridge_led_remove(const char *deviceId);
    int bridge_led_set_on(const char *deviceId);
    int bridge_led_set_off(const char *deviceId);
    int bridge_led_toggle(const char *deviceId);
    int bridge_led_get_state(const char *deviceId);

    // 蜂鸣器控制
    int bridge_buzzer_add(const char *deviceId, const char *gpioPath);
    int bridge_buzzer_on(const char *deviceId);
    int bridge_buzzer_off(const char *deviceId);
    int bridge_buzzer_beep(const char *deviceId, int durationMs);
    int bridge_buzzer_beep_pattern(const char *deviceId, int onMs, int offMs, int count);

    // 温湿度传感器
    int bridge_dht11_add(const char *deviceId, const char *devPath);
    int bridge_dht11_get_temp(const char *deviceId);
    int bridge_dht11_get_humi(const char *deviceId);

    // 温湿度上报
    void bridge_sensor_report_enable(int enable, int intervalSec);
    void bridge_sensor_report_once(void);

    // MQTT 连接
    int bridge_mqtt_connect(const char *host, int port, const char *clientId);
    void bridge_mqtt_disconnect(void);
    int bridge_mqtt_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif
