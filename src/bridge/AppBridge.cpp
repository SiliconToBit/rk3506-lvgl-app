#include "AppBridge.h"
#include "AppConfig.h"
#include "Backlight.h"
#include "DeviceService.h"
#include "IoTService.h"
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

// ========== 音乐播放 ==========

void bridge_music_scan_dir(const char* path)
{
    std::cout << "[Bridge] Scan music directory: " << path << std::endl;
    MusicPlayer::getInstance().scanDirectory(path);

    if (!MusicPlayer::getInstance().getPlaylist().empty())
    {
        MusicPlayer::getInstance().loadMusic(0);
    }
}

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

void bridge_music_play(void)
{
    std::cout << "[Bridge] Play music" << std::endl;
    MusicPlayer::getInstance().play();
}

void bridge_music_pause(void)
{
    std::cout << "[Bridge] Pause music" << std::endl;
    MusicPlayer::getInstance().pause();
}

void bridge_music_prev(void)
{
    std::cout << "[Bridge] Prev music" << std::endl;
    MusicPlayer::getInstance().prev();
}

void bridge_music_next(void)
{
    std::cout << "[Bridge] Next music" << std::endl;
    MusicPlayer::getInstance().next();
}

double bridge_music_current_time(void)
{
    return MusicPlayer::getInstance().getMusicCurrentTime();
}

double bridge_music_duration(void)
{
    return MusicPlayer::getInstance().getMusicDuration();
}

char* bridge_current_music_name(void)
{
    std::string music_name = MusicPlayer::getInstance().getCurrentSongName();
    char* c_str = (char*) std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

char* bridge_current_song_lyrics(void)
{
    std::string lyrics = MusicPlayer::getInstance().getCurrentSongLyrics();
    char* c_str = (char*) std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

char* bridge_get_current_lyric_line(void)
{
    double time = MusicPlayer::getInstance().getMusicCurrentTime();
    std::string line = MusicPlayer::getInstance().getCurrentLyricLine(time);
    char* c_str = (char*) std::malloc(line.size() + 1);
    std::strcpy(c_str, line.c_str());
    return c_str;
}

char* bridge_get_current_album_cover_path(void)
{
    std::string path = MusicPlayer::getInstance().getCurrentAlbumCoverPath();
    if (path.empty())
        return nullptr;
    char* c_str = (char*) std::malloc(path.size() + 1);
    std::strcpy(c_str, path.c_str());
    return c_str;
}

void bridge_set_volume(long volume)
{
    MusicPlayer::getInstance().setVolume(volume);
}

// ========== 天气 ==========

void bridge_update_weather(void)
{
    std::cout << "[Bridge] Update weather (Default: " APP_DEFAULT_CITY ")" << std::endl;
    WeatherService::getInstance().updateAsync(APP_DEFAULT_CITY);
}

void bridge_update_weather_city(const char* city)
{
    if (city && strlen(city) > 0)
    {
        std::cout << "[Bridge] Update weather for city: " << city << std::endl;
        WeatherService::getInstance().updateAsync(city);
    }
}

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

// ========== 温湿度 (通过 DHT11 单例) ==========

int bridge_get_temp(void)
{
    return Dht11::getInstance().readTemperature().value_or(0);
}

int bridge_get_humi(void)
{
    return Dht11::getInstance().readHumidity().value_or(0);
}

// ========== 背光控制 ==========

void bridge_set_brightness(int level)
{
    Backlight::getInstance().setBrightness(level);
}

int bridge_get_brightness(void)
{
    return Backlight::getInstance().getBrightness();
}

// ========== 设备控制 (通过 DeviceService) ==========

int bridge_led_add(const char* deviceId, const char* gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addLed(deviceId, gpioPath) ? 0 : -1;
}

int bridge_led_remove(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().removeDevice(deviceId) ? 0 : -1;
}

int bridge_led_set_on(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOn(deviceId) ? 0 : -1;
}

int bridge_led_set_off(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOff(deviceId) ? 0 : -1;
}

int bridge_led_toggle(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().toggleDevice(deviceId) ? 0 : -1;
}

int bridge_led_get_state(const char* deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getDeviceState(deviceId) ? 1 : 0;
}

int bridge_buzzer_add(const char* deviceId, const char* gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addBuzzer(deviceId, gpioPath) ? 0 : -1;
}

int bridge_buzzer_on(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOn(deviceId) ? 0 : -1;
}

int bridge_buzzer_off(const char* deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOff(deviceId) ? 0 : -1;
}

int bridge_buzzer_beep(const char* deviceId, int durationMs)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeep(deviceId, durationMs) ? 0 : -1;
}

int bridge_buzzer_beep_pattern(const char* deviceId, int onMs, int offMs, int count)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeepPattern(deviceId, onMs, offMs, count) ? 0 : -1;
}

int bridge_dht11_add(const char* deviceId, const char* devPath)
{
    if (!deviceId || !devPath)
        return -1;

    return DeviceService::instance().addDht11(deviceId, devPath) ? 0 : -1;
}

int bridge_dht11_get_temp(const char* deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getTemperature(deviceId);
}

int bridge_dht11_get_humi(const char* deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getHumidity(deviceId);
}

// ========== MQTT (通过 IoTService) ==========

int bridge_mqtt_connect(const char* host, int port, const char* clientId)
{
    if (!host)
        return -1;

    std::string id = clientId ? clientId : "lvgl_device";
    return IoTService::instance().connectMqtt(host, port, id) ? 0 : -1;
}

void bridge_mqtt_disconnect(void)
{
    IoTService::instance().disconnectMqtt();
}

int bridge_mqtt_is_connected(void)
{
    return IoTService::instance().isMqttConnected() ? 1 : 0;
}

void bridge_sensor_report_enable(int enable, int intervalSec)
{
    IoTService::instance().enableSensorReport(enable != 0, intervalSec);
}

void bridge_sensor_report_once(void)
{
    // 从 DeviceService 获取数据并通过 IoTService 上报
    auto& deviceService = DeviceService::instance();
    int temp = deviceService.getTemperature("dht11_1");
    int humi = deviceService.getHumidity("dht11_1");
    IoTService::instance().reportSensorData(temp, humi);
}

// ========== 红外学习模块 ==========

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

    if (!s_irDevice->openDevice())
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

void bridge_ir_deinit(void)
{
    if (s_irDevice)
    {
        delete s_irDevice;
        s_irDevice = nullptr;
        std::cout << "[Bridge] IR device deinitialized" << std::endl;
    }
}

int bridge_ir_start_learn(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->startLearn(index) ? 0 : -1;
}

void bridge_ir_stop_learn(void)
{
    if (!s_irDevice)
        return;

    s_irDevice->stopLearn();
}

int bridge_ir_get_learn_status(void)
{
    if (!s_irDevice)
        return -1;

    return static_cast<int>(s_irDevice->getLearnStatus());
}

int bridge_ir_emit(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->emitCode(index) ? 0 : -1;
}

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

int bridge_ir_clear(uint8_t index)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->clearCode(index) ? 0 : -1;
}

int bridge_ir_clear_all(void)
{
    if (!s_irDevice)
    {
        std::cerr << "[Bridge] IR device not initialized" << std::endl;
        return -1;
    }

    return s_irDevice->clearAllCodes() ? 0 : -1;
}

void bridge_ir_set_learn_complete_callback(IRLearnCompleteCallback callback)
{
    s_irLearnCompleteCallback = callback;
}

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
