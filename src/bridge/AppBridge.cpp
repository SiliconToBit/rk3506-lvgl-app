#include "AppBridge.h"
#include "AppConfig.h"
#include "Backlight.h"
#include "DeviceService.h"
#include "Dht11.h"
#include "IRDevice.h"
#include "IRCommandManager.h"
#include "MusicPlayer.h"
#include "WeatherService.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static IRDevice* s_irDevice = nullptr;

// 红外学习回调
static IRLearnCompleteCallback s_irLearnCompleteCallback = nullptr;
static IRLearnStatusCallback s_irStatusCallback = nullptr;

/**
 * @brief 初始化桥接层
 * @return 0 成功, -1 失败
 * @details 创建音乐播放器、DHT11传感器、背光控制器和天气服务实例
 */
int bridge_init(void)
{
    // Dht11、Backlight、WeatherService、MusicPlayer 都是单例模式
    // 在首次 getInstance() 时自动初始化
    if (!Dht11::getInstance().open())
    {
        std::cerr << "[Bridge] Failed to init DHT11" << std::endl;
    }

    std::cout << "[Bridge] DHT11 ready (singleton)" << std::endl;
    std::cout << "[Bridge] Backlight ready (singleton)" << std::endl;
    std::cout << "[Bridge] WeatherService ready (singleton)" << std::endl;

    // 初始化红外学习模块（使用单例，与DeviceService共享）
    s_irDevice = &IRDevice::getInstance();
    if (s_irDevice->open())
    {
        std::cout << "[Bridge] IR device initialized (singleton): " << APP_DEV_IR << std::endl;

        // 设置回调
        s_irDevice->setOnLearnComplete(
            [](const IRCode& code)
            {
                if (s_irLearnCompleteCallback)
                {
                    s_irLearnCompleteCallback(code.index, code.data.data(), code.data.size());
                }
            });

        s_irDevice->setOnStatusChange(
            [](IRLearnStatus status)
            {
                if (s_irStatusCallback)
                {
                    s_irStatusCallback(static_cast<int>(status));
                }
            });
    }
    else
    {
        std::cerr << "[Bridge] Warning: Failed to init IR device (optional), isOpen=" << s_irDevice->isOpen()
                  << std::endl;
        // 红外模块失败不影响整体初始化
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
    s_irDevice = nullptr;

    // Dht11、Backlight、WeatherService、MusicPlayer 都是单例
    // 不需要手动删除，程序结束时自动销毁

    std::cout << "[Bridge] Deinitialized" << std::endl;
}

/**
 * @brief 扫描音乐目录
 * @param path 音乐目录路径
 * @details 扫描指定目录下的音乐文件并加载第一首
 */
void bridge_music_scan_dir(const char* path)
{
    std::cout << "[Bridge] Scan music directory: " << path << std::endl;
    MusicPlayer::getInstance().scanDirectory(path);

    if (!MusicPlayer::getInstance().getPlaylist().empty())
    {
        MusicPlayer::getInstance().loadMusic(0);
    }
}

/**
 * @brief 获取音乐播放列表
 * @param out_count 输出参数,返回歌曲数量
 * @return char** 歌曲名称数组,需要调用bridge_free_music_playlist释放
 * @details 扫描默认音乐目录并返回播放列表
 */
char** bridge_get_music_playlist(size_t* out_count)
{
    bridge_music_scan_dir(APP_MUSIC_DIR);

    std::vector<std::string> playlist = MusicPlayer::getInstance().getPlaylist();
    *out_count = playlist.size();

    if (playlist.empty())
        return nullptr;

    char** c_array = (char**) std::malloc(playlist.size() * sizeof(char*));
    for (size_t i = 0; i < playlist.size(); ++i)
    {
        c_array[i] = (char*) std::malloc(playlist[i].size() + 1);
        std::strcpy(c_array[i], playlist[i].c_str());
    }
    return c_array;
}

/**
 * @brief 释放音乐播放列表内存
 * @param playlist 播放列表数组
 * @param count 歌曲数量
 */
void bridge_free_music_playlist(char** playlist, size_t count)
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
    std::cout << "[Bridge] Play music" << std::endl;
    MusicPlayer::getInstance().play();
}

/**
 * @brief 暂停音乐
 */
void bridge_music_pause(void)
{
    std::cout << "[Bridge] Pause music" << std::endl;
    MusicPlayer::getInstance().pause();
}

/**
 * @brief 上一首音乐
 */
void bridge_music_prev(void)
{
    std::cout << "[Bridge] Prev music" << std::endl;
    MusicPlayer::getInstance().prev();
}

/**
 * @brief 下一首音乐
 */
void bridge_music_next(void)
{
    std::cout << "[Bridge] Next music" << std::endl;
    MusicPlayer::getInstance().next();
}

/**
 * @brief 获取当前播放时间
 * @return double 当前播放时间(秒)
 */
double bridge_music_current_time(void)
{
    return MusicPlayer::getInstance().getMusicCurrentTime();
}

/**
 * @brief 获取音乐总时长
 * @return double 音乐总时长(秒)
 */
double bridge_music_duration(void)
{
    return MusicPlayer::getInstance().getMusicDuration();
}

/**
 * @brief 获取当前歌曲名称
 * @return char* 歌曲名称字符串,需要调用者free释放
 */
char* bridge_current_music_name(void)
{
    std::string music_name = MusicPlayer::getInstance().getCurrentSongName();
    char* c_str = (char*) std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

/**
 * @brief 获取当前歌曲歌词
 * @return char* 歌词字符串,需要调用者free释放
 */
char* bridge_current_song_lyrics(void)
{
    std::string lyrics = MusicPlayer::getInstance().getCurrentSongLyrics();
    char* c_str = (char*) std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

/**
 * @brief 获取当前时间对应的歌词行
 * @return char* 当前歌词行字符串,需要调用者free释放
 */
char* bridge_get_current_lyric_line(void)
{
    double time = MusicPlayer::getInstance().getMusicCurrentTime();
    std::string line = MusicPlayer::getInstance().getCurrentLyricLine(time);
    char* c_str = (char*) std::malloc(line.size() + 1);
    std::strcpy(c_str, line.c_str());
    return c_str;
}

/**
 * @brief 获取当前歌曲专辑封面路径
 * @return char* 封面图片路径,需要调用者free释放;无封面返回nullptr
 */
char* bridge_get_current_album_cover_path(void)
{
    std::string path = MusicPlayer::getInstance().getCurrentAlbumCoverPath();
    if (path.empty())
        return nullptr;
    char* c_str = (char*) std::malloc(path.size() + 1);
    std::strcpy(c_str, path.c_str());
    return c_str;
}

/**
 * @brief 设置音量
 * @param volume 音量值(0-100)
 */
void bridge_set_volume(long volume)
{
    MusicPlayer::getInstance().setVolume(volume);
}

/**
 * @brief 更新天气数据(默认城市)
 * @details 异步获取默认城市的天气数据
 */
void bridge_update_weather(void)
{
    std::cout << "[Bridge] Update weather (Default: " APP_DEFAULT_CITY ")" << std::endl;
    WeatherService::getInstance().updateAsync(APP_DEFAULT_CITY);
}

/**
 * @brief 更新指定城市的天气数据
 * @param city 城市名称
 * @details 异步获取指定城市的天气数据
 */
void bridge_update_weather_city(const char* city)
{
    if (city && strlen(city) > 0)
    {
        std::cout << "[Bridge] Update weather for city: " << city << std::endl;
        WeatherService::getInstance().updateAsync(city);
    }
}

/**
 * @brief 获取天气数据
 * @param data 输出参数,存储天气数据结构
 * @details 将C++ WeatherService数据转换为C语言AppWeatherData结构
 */
void bridge_get_weather_data(AppWeatherData* data)
{
    if (!data)
        return;

    WeatherService& service = WeatherService::getInstance();
    CurrentWeather current = service.getCurrent();
    std::vector<ForecastWeather> forecast = service.getForecast();

    strncpy(data->city, current.city.c_str(), sizeof(data->city) - 1);
    data->city[sizeof(data->city) - 1] = '\0';

    strncpy(data->weather, current.description.c_str(), sizeof(data->weather) - 1);
    data->weather[sizeof(data->weather) - 1] = '\0';

    data->iconCode = current.iconCode;
    data->temperature = current.temperature;
    data->humidity = current.humidity;
    data->feelsLike = current.feelsLike;

    strncpy(data->windDir, current.windDir.c_str(), sizeof(data->windDir) - 1);
    data->windDir[sizeof(data->windDir) - 1] = '\0';

    data->windSpeed = current.windSpeed;

    // 填充预报数据
    if (forecast.size() > 0)
    {
        data->day1_iconCode = forecast[0].iconCode;
        data->day1_tempMin = forecast[0].tempMin;
        data->day1_tempMax = forecast[0].tempMax;
        strncpy(data->day1_windDir, forecast[0].windDir.c_str(), sizeof(data->day1_windDir) - 1);
        data->day1_windDir[sizeof(data->day1_windDir) - 1] = '\0';
        strncpy(data->day1_fxDate, forecast[0].date.c_str(), sizeof(data->day1_fxDate) - 1);
        data->day1_fxDate[sizeof(data->day1_fxDate) - 1] = '\0';
    }

    if (forecast.size() > 1)
    {
        data->day2_iconCode = forecast[1].iconCode;
        data->day2_tempMin = forecast[1].tempMin;
        data->day2_tempMax = forecast[1].tempMax;
        strncpy(data->day2_windDir, forecast[1].windDir.c_str(), sizeof(data->day2_windDir) - 1);
        data->day2_windDir[sizeof(data->day2_windDir) - 1] = '\0';
    }

    if (forecast.size() > 2)
    {
        data->day3_iconCode = forecast[2].iconCode;
        data->day3_tempMin = forecast[2].tempMin;
        data->day3_tempMax = forecast[2].tempMax;
        strncpy(data->day3_windDir, forecast[2].windDir.c_str(), sizeof(data->day3_windDir) - 1);
        data->day3_windDir[sizeof(data->day3_windDir) - 1] = '\0';
    }
}

/**
 * @brief 读取温度(兼容接口)
 * @return int 温度值(摄氏度)
 * @details 使用 DHT11 单例读取温度
 */
int bridge_get_temp(void)
{
    return Dht11::getInstance().readTemperature();
}

/**
 * @brief 读取湿度(兼容接口)
 * @return int 湿度值(%)
 * @details 使用 DHT11 单例读取湿度
 */
int bridge_get_humi(void)
{
    return Dht11::getInstance().readHumidity();
}

/**
 * @brief 设置屏幕亮度
 * @param level 亮度等级(0-100)
 * @details 使用 Backlight 单例设置亮度
 */
void bridge_set_brightness(int level)
{
    Backlight::getInstance().setBrightness(level);
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
int bridge_led_add(const char* deviceId, const char* gpioPath)
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
int bridge_led_remove(const char* deviceId)
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
int bridge_led_set_on(const char* deviceId)
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
int bridge_led_set_off(const char* deviceId)
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
int bridge_led_toggle(const char* deviceId)
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
int bridge_led_get_state(const char* deviceId)
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
int bridge_buzzer_add(const char* deviceId, const char* gpioPath)
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
int bridge_buzzer_on(const char* deviceId)
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
int bridge_buzzer_off(const char* deviceId)
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
int bridge_buzzer_beep(const char* deviceId, int durationMs)
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
int bridge_buzzer_beep_pattern(const char* deviceId, int onMs, int offMs, int count)
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
int bridge_dht11_add(const char* deviceId, const char* devPath)
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
int bridge_dht11_get_temp(const char* deviceId)
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
int bridge_dht11_get_humi(const char* deviceId)
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
int bridge_mqtt_connect(const char* host, int port, const char* clientId)
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

/**
 * @brief 初始化红外学习模块
 * @param devPath 串口设备路径,如"/dev/ttyS1"
 * @return 0 成功, -1 失败
 */
int bridge_ir_init(const char* devPath)
{
    if (!devPath)
        return -1;

    if (s_irDevice)
    {
        delete s_irDevice;
        s_irDevice = nullptr;
    }

    s_irDevice = new (std::nothrow) IRDevice(devPath);
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] Failed to create IRDevice" << std::endl;
        return -1;
    }

    if (!s_irDevice->open())
    {
        std::cerr << "[Bridge] Failed to open IR device: " << devPath << std::endl;
        delete s_irDevice;
        s_irDevice = nullptr;
        return -1;
    }

    // 设置回调
    s_irDevice->setOnLearnComplete(
        [](const IRCode& code)
        {
            if (s_irLearnCompleteCallback)
            {
                s_irLearnCompleteCallback(code.index, code.data.data(), code.data.size());
            }
        });

    s_irDevice->setOnStatusChange(
        [](IRLearnStatus status)
        {
            if (s_irStatusCallback)
            {
                s_irStatusCallback(static_cast<int>(status));
            }
        });

    std::cout << "[Bridge] IR device initialized: " << devPath << std::endl;
    return 0;
}

/**
 * @brief 反初始化红外学习模块
 */
void bridge_ir_deinit(void)
{
    if (s_irDevice)
    {
        delete s_irDevice;
        s_irDevice = nullptr;
        std::cout << "[Bridge] IR device deinitialized" << std::endl;
    }
}

/**
 * @brief 开始红外学习
 * @param index 存储索引(0-255)
 * @return 0 成功, -1 失败
 */
int bridge_ir_start_learn(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->startLearn(index) ? 0 : -1;
}

/**
 * @brief 停止红外学习
 */
void bridge_ir_stop_learn(void)
{
    if (!s_irDevice)
        return;

    s_irDevice->stopLearn();
}

/**
 * @brief 获取红外学习状态
 * @return 0 空闲, 1 学习中, 2 成功, 3 超时, 4 错误, -1 未初始化
 */
int bridge_ir_get_learn_status(void)
{
    if (!s_irDevice)
        return -1;

    return static_cast<int>(s_irDevice->getLearnStatus());
}

/**
 * @brief 发射存储的红外码
 * @param index 存储索引
 * @return 0 成功, -1 失败
 */
int bridge_ir_emit(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->emitCode(index) ? 0 : -1;
}

/**
 * @brief 发射原始红外码
 * @param data 红外码数据
 * @param len 数据长度
 * @return 0 成功, -1 失败
 */
int bridge_ir_emit_raw(const uint8_t* data, size_t len)
{
    if (!s_irDevice || !data || len == 0)
    {
        std::cerr << "[Bridge] IR device not initialized or invalid data" << std::endl;
        return -1;
    }

    std::vector<uint8_t> code(data, data + len);
    return s_irDevice->emitRawCode(code) ? 0 : -1;
}

/**
 * @brief 清除指定索引的红外码
 * @param index 存储索引
 * @return 0 成功, -1 失败
 */
int bridge_ir_clear(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->clearCode(index) ? 0 : -1;
}

/**
 * @brief 清除所有红外码
 * @return 0 成功, -1 失败
 */
int bridge_ir_clear_all(void)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->clearAllCodes() ? 0 : -1;
}

/**
 * @brief 设置红外学习完成回调
 * @param callback 回调函数
 */
void bridge_ir_set_learn_complete_callback(IRLearnCompleteCallback callback)
{
    s_irLearnCompleteCallback = callback;
}

/**
 * @brief 设置红外学习状态回调
 * @param callback 回调函数
 */
void bridge_ir_set_status_callback(IRLearnStatusCallback callback)
{
    s_irStatusCallback = callback;
}

// ========== 红外命令管理 ==========

static IRExtLearnSaveCallback s_irExtLearnSaveCallback = nullptr;
static std::string s_extLearnDeviceName;
static std::string s_extLearnCommandName;

int bridge_ir_cmd_init(const char* dataPath)
{
    std::string path = dataPath ? dataPath : "/data/ir_commands";
    return IRCommandManager::getInstance().init(path) ? 0 : -1;
}

void bridge_ir_cmd_deinit(void)
{
    IRCommandManager::getInstance().deinit();
}

int bridge_ir_cmd_add(const char* deviceName, const char* commandName, const uint8_t* data, size_t len)
{
    if (!deviceName || !commandName || !data || len == 0)
        return -1;

    std::vector<uint8_t> code(data, data + len);
    return IRCommandManager::getInstance().addCommand(deviceName, commandName, code) ? 0 : -1;
}

int bridge_ir_cmd_remove(const char* deviceName, const char* commandName)
{
    if (!deviceName || !commandName)
        return -1;

    return IRCommandManager::getInstance().removeCommand(deviceName, commandName) ? 0 : -1;
}

int bridge_ir_cmd_remove_device(const char* deviceName)
{
    if (!deviceName)
        return -1;

    return IRCommandManager::getInstance().removeDevice(deviceName) ? 0 : -1;
}

int bridge_ir_cmd_emit(const char* deviceName, const char* commandName)
{
    if (!deviceName || !commandName)
        return -1;

    return IRCommandManager::getInstance().emitCommand(deviceName, commandName) ? 0 : -1;
}

int bridge_ir_cmd_get(const char* deviceName, const char* commandName, uint8_t* outData, size_t* inOutLen)
{
    if (!deviceName || !commandName || !inOutLen)
        return -1;

    auto data = IRCommandManager::getInstance().getCommand(deviceName, commandName);
    if (data.empty())
        return -1;

    if (outData && *inOutLen >= data.size())
    {
        std::memcpy(outData, data.data(), data.size());
    }
    *inOutLen = data.size();
    return 0;
}

char** bridge_ir_cmd_get_devices(size_t* outCount)
{
    if (!outCount)
        return nullptr;

    auto devices = IRCommandManager::getInstance().getDeviceList();
    *outCount = devices.size();

    if (devices.empty())
        return nullptr;

    char** c_array = (char**) std::malloc(devices.size() * sizeof(char*));
    for (size_t i = 0; i < devices.size(); ++i)
    {
        c_array[i] = (char*) std::malloc(devices[i].size() + 1);
        std::strcpy(c_array[i], devices[i].c_str());
    }
    return c_array;
}

void bridge_ir_cmd_free_devices(char** devices, size_t count)
{
    if (devices == nullptr)
        return;
    for (size_t i = 0; i < count; ++i)
    {
        if (devices[i] != nullptr)
            std::free(devices[i]);
    }
    std::free(devices);
}

char** bridge_ir_cmd_get_commands(const char* deviceName, size_t* outCount)
{
    if (!deviceName || !outCount)
        return nullptr;

    auto commands = IRCommandManager::getInstance().getCommandList(deviceName);
    *outCount = commands.size();

    if (commands.empty())
        return nullptr;

    char** c_array = (char**) std::malloc(commands.size() * sizeof(char*));
    for (size_t i = 0; i < commands.size(); ++i)
    {
        c_array[i] = (char*) std::malloc(commands[i].size() + 1);
        std::strcpy(c_array[i], commands[i].c_str());
    }
    return c_array;
}

void bridge_ir_cmd_free_commands(char** commands, size_t count)
{
    if (commands == nullptr)
        return;
    for (size_t i = 0; i < count; ++i)
    {
        if (commands[i] != nullptr)
            std::free(commands[i]);
    }
    std::free(commands);
}

int bridge_ir_cmd_exists(const char* deviceName, const char* commandName)
{
    if (!deviceName || !commandName)
        return 0;

    return IRCommandManager::getInstance().commandExists(deviceName, commandName) ? 1 : 0;
}

int bridge_ir_ext_learn_and_save(const char* deviceName, const char* commandName, IRExtLearnSaveCallback callback)
{
    if (!deviceName || !commandName || !s_irDevice)
        return -1;

    s_extLearnDeviceName = deviceName;
    s_extLearnCommandName = commandName;
    s_irExtLearnSaveCallback = callback;

    s_irDevice->setOnExtLearnComplete(
        [](const std::vector<uint8_t>& data)
        {
            if (!data.empty())
            {
                IRCommandManager::getInstance().addCommand(s_extLearnDeviceName, s_extLearnCommandName, data);
                if (s_irExtLearnSaveCallback)
                {
                    s_irExtLearnSaveCallback(1, s_extLearnDeviceName.c_str(), s_extLearnCommandName.c_str());
                }
            }
            else
            {
                if (s_irExtLearnSaveCallback)
                {
                    s_irExtLearnSaveCallback(0, s_extLearnDeviceName.c_str(), s_extLearnCommandName.c_str());
                }
            }
        });

    return s_irDevice->startExtLearn() ? 0 : -1;
}
