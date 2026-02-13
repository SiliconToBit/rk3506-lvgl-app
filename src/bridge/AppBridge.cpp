#include "AppBridge.h"
#include "AppConfig.h"
#include "Backlight.h"
#include "DeviceService.h"
#include "Dht11.h"
#include "MusicPlayer.h"
#include "WeatherService.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static MusicPlayer *s_player = nullptr;
static Dht11 *s_dht11 = nullptr;
static Backlight *s_backlight = nullptr;
static WeatherService *s_weatherService = nullptr;

/**
 * @brief 初始化桥接层
 * @return 0 成功, -1 失败
 * @details 创建音乐播放器、DHT11传感器、背光控制器和天气服务实例
 */
int bridge_init(void)
{
    s_player = new (std::nothrow) MusicPlayer();
    if (!s_player)
    {
        std::cerr << "[Bridge] Failed to create MusicPlayer" << std::endl;
        return -1;
    }

    s_dht11 = new (std::nothrow) Dht11(APP_DEV_DHT11);
    if (!s_dht11 || !s_dht11->open())
    {
        std::cerr << "[Bridge] Failed to init DHT11" << std::endl;
    }

    s_backlight = new (std::nothrow) Backlight(APP_DEV_BACKLIGHT);
    if (!s_backlight)
    {
        std::cerr << "[Bridge] Failed to create Backlight" << std::endl;
    }

    s_weatherService = new (std::nothrow) WeatherService(APP_WEATHER_API_KEY);
    if (!s_weatherService)
    {
        std::cerr << "[Bridge] Failed to create WeatherService" << std::endl;
        return -1;
    }

    std::cout << "[Bridge] Initialized successfully" << std::endl;
    return 0;
}

/**
 * @brief 反初始化桥接层
 * @details 释放所有创建的实例,置空指针
 */
void bridge_deinit(void)
{
    delete s_weatherService;
    s_weatherService = nullptr;

    delete s_backlight;
    s_backlight = nullptr;

    delete s_dht11;
    s_dht11 = nullptr;

    delete s_player;
    s_player = nullptr;

    std::cout << "[Bridge] Deinitialized" << std::endl;
}

/**
 * @brief 扫描音乐目录
 * @param path 音乐目录路径
 * @details 扫描指定目录下的音乐文件并加载第一首
 */
void bridge_music_scan_dir(const char *path)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Scan music directory: " << path << std::endl;
    s_player->scanDirectory(path);

    if (!s_player->getPlaylist().empty())
    {
        s_player->loadMusic(0);
    }
}

/**
 * @brief 获取音乐播放列表
 * @param out_count 输出参数,返回歌曲数量
 * @return char** 歌曲名称数组,需要调用bridge_free_music_playlist释放
 * @details 扫描默认音乐目录并返回播放列表
 */
char **bridge_get_music_playlist(size_t *out_count)
{
    bridge_music_scan_dir(APP_MUSIC_DIR);

    std::vector<std::string> playlist = s_player->getPlaylist();
    *out_count = playlist.size();

    if (playlist.empty())
        return nullptr;

    char **c_array = (char **)std::malloc(playlist.size() * sizeof(char *));
    for (size_t i = 0; i < playlist.size(); ++i)
    {
        c_array[i] = (char *)std::malloc(playlist[i].size() + 1);
        std::strcpy(c_array[i], playlist[i].c_str());
    }
    return c_array;
}

/**
 * @brief 释放音乐播放列表内存
 * @param playlist 播放列表数组
 * @param count 歌曲数量
 */
void bridge_free_music_playlist(char **playlist, size_t count)
{
    if (playlist == nullptr)
        return;
    for (size_t i = 0; i < count; ++i)
    {
        if (playlist[i] != nullptr)
            std::free(playlist[i]);
    }
    std::free(playlist);
}

/**
 * @brief 播放音乐
 */
void bridge_music_play(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Play music" << std::endl;
    s_player->play();
}

/**
 * @brief 暂停音乐
 */
void bridge_music_pause(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Pause music" << std::endl;
    s_player->pause();
}

/**
 * @brief 上一首音乐
 */
void bridge_music_prev(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Prev music" << std::endl;
    s_player->prev();
}

/**
 * @brief 下一首音乐
 */
void bridge_music_next(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Next music" << std::endl;
    s_player->next();
}

/**
 * @brief 获取当前播放时间
 * @return double 当前播放时间(秒)
 */
double bridge_music_current_time(void)
{
    if (!s_player)
        return 0.0;
    return s_player->getMusicCurrentTime();
}

/**
 * @brief 获取音乐总时长
 * @return double 音乐总时长(秒)
 */
double bridge_music_duration(void)
{
    if (!s_player)
        return 0.0;
    return s_player->getMusicDuration();
}

/**
 * @brief 获取当前歌曲名称
 * @return char* 歌曲名称字符串,需要调用者free释放
 */
char *bridge_current_music_name(void)
{
    if (!s_player)
        return nullptr;
    std::string music_name = s_player->getCurrentSongName();
    char *c_str = (char *)std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

/**
 * @brief 获取当前歌曲歌词
 * @return char* 歌词字符串,需要调用者free释放
 */
char *bridge_current_song_lyrics(void)
{
    if (!s_player)
        return nullptr;
    std::string lyrics = s_player->getCurrentSongLyrics();
    char *c_str = (char *)std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

/**
 * @brief 获取当前时间对应的歌词行
 * @return char* 当前歌词行字符串,需要调用者free释放
 */
char *bridge_get_current_lyric_line(void)
{
    if (!s_player)
        return nullptr;
    double time = s_player->getMusicCurrentTime();
    std::string line = s_player->getCurrentLyricLine(time);
    char *c_str = (char *)std::malloc(line.size() + 1);
    std::strcpy(c_str, line.c_str());
    return c_str;
}

/**
 * @brief 获取当前歌曲专辑封面路径
 * @return char* 封面图片路径,需要调用者free释放;无封面返回nullptr
 */
char *bridge_get_current_album_cover_path(void)
{
    if (!s_player)
        return nullptr;
    std::string path = s_player->getCurrentAlbumCoverPath();
    if (path.empty())
        return nullptr;
    char *c_str = (char *)std::malloc(path.size() + 1);
    std::strcpy(c_str, path.c_str());
    return c_str;
}

/**
 * @brief 设置音量
 * @param volume 音量值(0-100)
 */
void bridge_set_volume(long volume)
{
    if (!s_player)
        return;
    s_player->setVolume(volume);
}

/**
 * @brief 更新天气数据(默认城市)
 * @details 异步获取默认城市的天气数据
 */
void bridge_update_weather(void)
{
    if (!s_weatherService)
        return;
    std::cout << "[Bridge] Update weather (Default: " APP_DEFAULT_CITY ")" << std::endl;
    s_weatherService->updateWeatherAsync(APP_DEFAULT_CITY);
}

/**
 * @brief 更新指定城市的天气数据
 * @param city 城市名称
 * @details 异步获取指定城市的天气数据
 */
void bridge_update_weather_city(const char *city)
{
    if (!s_weatherService)
        return;
    if (city && strlen(city) > 0)
    {
        std::cout << "[Bridge] Update weather for city: " << city << std::endl;
        s_weatherService->updateWeatherAsync(city);
    }
}

/**
 * @brief 获取天气数据
 * @param data 输出参数,存储天气数据结构
 * @details 将C++ WeatherData转换为C语言AppWeatherData结构
 */
void bridge_get_weather_data(AppWeatherData *data)
{
    if (!data)
        return;
    if (!s_weatherService)
        return;

    WeatherData cppData = s_weatherService->getWeatherData();

    strncpy(data->city, cppData.city, sizeof(data->city) - 1);
    data->city[sizeof(data->city) - 1] = '\0';

    strncpy(data->weather, cppData.weather, sizeof(data->weather) - 1);
    data->weather[sizeof(data->weather) - 1] = '\0';

    data->iconCode = cppData.iconCode;
    data->temperature = cppData.temperature;
    data->humidity = cppData.humidity;
    data->feelsLike = cppData.feelsLike;

    strncpy(data->windDir, cppData.windDir, sizeof(data->windDir) - 1);
    data->windDir[sizeof(data->windDir) - 1] = '\0';

    data->windSpeed = cppData.windSpeed;

    data->day1_iconCode = cppData.day1_iconCode;
    data->day1_tempMin = cppData.day1_tempMin;
    data->day1_tempMax = cppData.day1_tempMax;
    strncpy(data->day1_windDir, cppData.day1_windDir, sizeof(data->day1_windDir) - 1);
    data->day1_windDir[sizeof(data->day1_windDir) - 1] = '\0';
    strncpy(data->day1_fxDate, cppData.day1_fxDate, sizeof(data->day1_fxDate) - 1);
    data->day1_fxDate[sizeof(data->day1_fxDate) - 1] = '\0';

    data->day2_iconCode = cppData.day2_iconCode;
    data->day2_tempMin = cppData.day2_tempMin;
    data->day2_tempMax = cppData.day2_tempMax;
    strncpy(data->day2_windDir, cppData.day2_windDir, sizeof(data->day2_windDir) - 1);
    data->day2_windDir[sizeof(data->day2_windDir) - 1] = '\0';

    data->day3_iconCode = cppData.day3_iconCode;
    data->day3_tempMin = cppData.day3_tempMin;
    data->day3_tempMax = cppData.day3_tempMax;
    strncpy(data->day3_windDir, cppData.day3_windDir, sizeof(data->day3_windDir) - 1);
    data->day3_windDir[sizeof(data->day3_windDir) - 1] = '\0';
}

/**
 * @brief 读取温度(兼容接口)
 * @return int 温度值(摄氏度)
 * @details 使用全局DHT11实例读取温度
 */
int bridge_get_temp(void)
{
    if (!s_dht11)
        return 0;
    return s_dht11->readTemperature();
}

/**
 * @brief 读取湿度(兼容接口)
 * @return int 湿度值(%)
 * @details 使用全局DHT11实例读取湿度
 */
int bridge_get_humi(void)
{
    if (!s_dht11)
        return 0;
    return s_dht11->readHumidity();
}

/**
 * @brief 设置屏幕亮度
 * @param level 亮度等级(0-100)
 */
void bridge_set_brightness(int level)
{
    if (!s_backlight)
        return;
    s_backlight->setBrightness(level);
}

/**
 * @brief 初始化设备服务
 * @return 0 成功, -1 失败
 * @details 创建DeviceService单例并初始化
 */
int bridge_device_init(void)
{
    return DeviceService::instance().init() ? 0 : -1;
}

/**
 * @brief 反初始化设备服务
 */
void bridge_device_deinit(void)
{
    DeviceService::instance().deinit();
}

/**
 * @brief 添加LED设备
 * @param deviceId 设备唯一标识符
 * @param gpioPath GPIO设备路径
 * @return 0 成功, -1 失败
 */
int bridge_led_add(const char *deviceId, const char *gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addLed(deviceId, gpioPath) ? 0 : -1;
}

/**
 * @brief 移除LED设备
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_led_remove(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().removeDevice(deviceId) ? 0 : -1;
}

/**
 * @brief 打开LED
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_led_set_on(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOn(deviceId) ? 0 : -1;
}

/**
 * @brief 关闭LED
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_led_set_off(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOff(deviceId) ? 0 : -1;
}

/**
 * @brief 切换LED状态
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_led_toggle(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().toggleDevice(deviceId) ? 0 : -1;
}

/**
 * @brief 获取LED状态
 * @param deviceId 设备唯一标识符
 * @return 1 打开, 0 关闭或设备不存在
 */
int bridge_led_get_state(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getDeviceState(deviceId) ? 1 : 0;
}

/**
 * @brief 添加蜂鸣器设备
 * @param deviceId 设备唯一标识符
 * @param gpioPath GPIO设备路径
 * @return 0 成功, -1 失败
 */
int bridge_buzzer_add(const char *deviceId, const char *gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addBuzzer(deviceId, gpioPath) ? 0 : -1;
}

/**
 * @brief 打开蜂鸣器
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_buzzer_on(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOn(deviceId) ? 0 : -1;
}

/**
 * @brief 关闭蜂鸣器
 * @param deviceId 设备唯一标识符
 * @return 0 成功, -1 失败
 */
int bridge_buzzer_off(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOff(deviceId) ? 0 : -1;
}

/**
 * @brief 蜂鸣器响一声
 * @param deviceId 设备唯一标识符
 * @param durationMs 响铃持续时间(毫秒)
 * @return 0 成功, -1 失败
 */
int bridge_buzzer_beep(const char *deviceId, int durationMs)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeep(deviceId, durationMs) ? 0 : -1;
}

/**
 * @brief 蜂鸣器响多声
 * @param deviceId 设备唯一标识符
 * @param onMs 每声持续时间(毫秒)
 * @param offMs 每声间隔时间(毫秒)
 * @param count 响声次数
 * @return 0 成功, -1 失败
 */
int bridge_buzzer_beep_pattern(const char *deviceId, int onMs, int offMs, int count)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeepPattern(deviceId, onMs, offMs, count) ? 0 : -1;
}

/**
 * @brief 添加DHT11传感器
 * @param deviceId 设备唯一标识符
 * @param devPath 设备文件路径
 * @return 0 成功, -1 失败
 */
int bridge_dht11_add(const char *deviceId, const char *devPath)
{
    if (!deviceId || !devPath)
        return -1;

    return DeviceService::instance().addDht11(deviceId, devPath) ? 0 : -1;
}

/**
 * @brief 读取DHT11温度
 * @param deviceId 设备唯一标识符
 * @return int 温度值(摄氏度),失败返回0
 */
int bridge_dht11_get_temp(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getTemperature(deviceId);
}

/**
 * @brief 读取DHT11湿度
 * @param deviceId 设备唯一标识符
 * @return int 湿度值(%),失败返回0
 */
int bridge_dht11_get_humi(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getHumidity(deviceId);
}

/**
 * @brief 启用/禁用传感器数据上报
 * @param enable 1启用, 0禁用
 * @param intervalSec 上报间隔(秒)
 */
void bridge_sensor_report_enable(int enable, int intervalSec)
{
    DeviceService::instance().enableSensorReport(enable != 0, intervalSec);
}

/**
 * @brief 单次传感器数据上报
 */
void bridge_sensor_report_once(void)
{
    DeviceService::instance().reportSensorData();
}

/**
 * @brief 连接MQTT服务器
 * @param host 服务器地址
 * @param port 端口号
 * @param clientId 客户端ID(可为NULL,使用默认值)
 * @return 0 成功, -1 失败
 */
int bridge_mqtt_connect(const char *host, int port, const char *clientId)
{
    if (!host)
        return -1;

    std::string id = clientId ? clientId : "lvgl_device";
    return DeviceService::instance().connectMqtt(host, port, id) ? 0 : -1;
}

/**
 * @brief 断开MQTT连接
 */
void bridge_mqtt_disconnect(void)
{
    DeviceService::instance().disconnectMqtt();
}

/**
 * @brief 检查MQTT连接状态
 * @return 1 已连接, 0 未连接
 */
int bridge_mqtt_is_connected(void)
{
    return DeviceService::instance().isMqttConnected() ? 1 : 0;
}
