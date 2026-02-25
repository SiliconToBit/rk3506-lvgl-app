#ifndef LVGL_APP_BRIDGE_APP_BRIDGE_H
#define LVGL_APP_BRIDGE_APP_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // ========== 音乐播放 ==========
    void bridge_music_scan_dir(const char *path);
    char **bridge_get_music_playlist(size_t *out_count);
    void bridge_free_music_playlist(char **playlist, size_t count);
    void bridge_music_play(void);
    void bridge_music_pause(void);
    void bridge_music_prev(void);
    void bridge_music_next(void);

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

    void bridge_update_weather(void);
    void bridge_update_weather_city(const char *city);
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

    // ========== 设备控制接口 ==========
    // 注意：设备初始化现在由 DeviceService 统一管理
    // 这些接口需要确保 DeviceService 已经初始化

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

    // ========== MQTT (通过 IoTService) ==========
    int bridge_mqtt_connect(const char *host, int port, const char *clientId);
    void bridge_mqtt_disconnect(void);
    int bridge_mqtt_is_connected(void);

    // 传感器上报控制 (通过 IoTService)
    void bridge_sensor_report_enable(int enable, int intervalSec);
    void bridge_sensor_report_once(void);

    // ========== 红外学习模块 ==========
    int bridge_ir_init(const char *devPath);
    void bridge_ir_deinit(void);

    // 红外学习功能
    int bridge_ir_start_learn(uint8_t index);
    void bridge_ir_stop_learn(void);
    int bridge_ir_get_learn_status(void);

    // 红外发射功能
    int bridge_ir_emit(uint8_t index);
    int bridge_ir_emit_raw(const uint8_t *data, size_t len);

    // 红外码存储管理
    int bridge_ir_clear(uint8_t index);
    int bridge_ir_clear_all(void);

    // 红外学习回调类型定义
    typedef void (*IRLearnCompleteCallback)(uint8_t index, const uint8_t *data, size_t len);
    typedef void (*IRLearnStatusCallback)(int status);

    // 设置红外学习回调
    void bridge_ir_set_learn_complete_callback(IRLearnCompleteCallback callback);
    void bridge_ir_set_status_callback(IRLearnStatusCallback callback);

    // ========== 红外命令管理 ==========
    
    // 初始化红外命令管理器
    int bridge_ir_cmd_init(const char *dataPath);
    void bridge_ir_cmd_deinit(void);

    // 添加红外命令 (设备名 + 命令名 + 原始数据)
    int bridge_ir_cmd_add(const char *deviceName, const char *commandName, 
                          const uint8_t *data, size_t len);
    
    // 删除红外命令
    int bridge_ir_cmd_remove(const char *deviceName, const char *commandName);
    
    // 删除整个设备的所有命令
    int bridge_ir_cmd_remove_device(const char *deviceName);

    // 发射红外命令
    int bridge_ir_cmd_emit(const char *deviceName, const char *commandName);
    
    // 获取红外命令数据
    int bridge_ir_cmd_get(const char *deviceName, const char *commandName, 
                         uint8_t *outData, size_t *inOutLen);

    // 获取设备列表
    char **bridge_ir_cmd_get_devices(size_t *outCount);
    void bridge_ir_cmd_free_devices(char **devices, size_t count);

    // 获取指定设备的命令列表
    char **bridge_ir_cmd_get_commands(const char *deviceName, size_t *outCount);
    void bridge_ir_cmd_free_commands(char **commands, size_t count);

    // 检查命令是否存在
    int bridge_ir_cmd_exists(const char *deviceName, const char *commandName);

    // ========== 外部学习并保存 ==========
    
    // 开始外部学习并保存到命令库
    typedef void (*IRExtLearnSaveCallback)(int success, const char *deviceName, const char *commandName);
    int bridge_ir_ext_learn_and_save(const char *deviceName, const char *commandName, 
                                      IRExtLearnSaveCallback callback);

#ifdef __cplusplus
}
#endif

#endif
