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

void bridge_music_play(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Play music" << std::endl;
    s_player->play();
}

void bridge_music_pause(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Pause music" << std::endl;
    s_player->pause();
}

void bridge_music_prev(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Prev music" << std::endl;
    s_player->prev();
}

void bridge_music_next(void)
{
    if (!s_player)
        return;
    std::cout << "[Bridge] Next music" << std::endl;
    s_player->next();
}

double bridge_music_current_time(void)
{
    if (!s_player)
        return 0.0;
    return s_player->getMusicCurrentTime();
}

double bridge_music_duration(void)
{
    if (!s_player)
        return 0.0;
    return s_player->getMusicDuration();
}

char *bridge_current_music_name(void)
{
    if (!s_player)
        return nullptr;
    std::string music_name = s_player->getCurrentSongName();
    char *c_str = (char *)std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

char *bridge_current_song_lyrics(void)
{
    if (!s_player)
        return nullptr;
    std::string lyrics = s_player->getCurrentSongLyrics();
    char *c_str = (char *)std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

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

void bridge_set_volume(long volume)
{
    if (!s_player)
        return;
    s_player->setVolume(volume);
}

void bridge_update_weather(void)
{
    if (!s_weatherService)
        return;
    std::cout << "[Bridge] Update weather (Default: " APP_DEFAULT_CITY ")" << std::endl;
    s_weatherService->updateWeatherAsync(APP_DEFAULT_CITY);
}

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

int bridge_get_temp(void)
{
    if (!s_dht11)
        return 0;
    return s_dht11->readTemperature();
}

int bridge_get_humi(void)
{
    if (!s_dht11)
        return 0;
    return s_dht11->readHumidity();
}

void bridge_set_brightness(int level)
{
    if (!s_backlight)
        return;
    s_backlight->setBrightness(level);
}

int bridge_device_init(void)
{
    return DeviceService::instance().init() ? 0 : -1;
}

void bridge_device_deinit(void)
{
    DeviceService::instance().deinit();
}

int bridge_led_add(const char *deviceId, const char *gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addLed(deviceId, gpioPath) ? 0 : -1;
}

int bridge_led_remove(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().removeDevice(deviceId) ? 0 : -1;
}

int bridge_led_set_on(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOn(deviceId) ? 0 : -1;
}

int bridge_led_set_off(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().setDeviceOff(deviceId) ? 0 : -1;
}

int bridge_led_toggle(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().toggleDevice(deviceId) ? 0 : -1;
}

int bridge_led_get_state(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getDeviceState(deviceId) ? 1 : 0;
}

int bridge_buzzer_add(const char *deviceId, const char *gpioPath)
{
    if (!deviceId || !gpioPath)
        return -1;

    return DeviceService::instance().addBuzzer(deviceId, gpioPath) ? 0 : -1;
}

int bridge_buzzer_on(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOn(deviceId) ? 0 : -1;
}

int bridge_buzzer_off(const char *deviceId)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerOff(deviceId) ? 0 : -1;
}

int bridge_buzzer_beep(const char *deviceId, int durationMs)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeep(deviceId, durationMs) ? 0 : -1;
}

int bridge_buzzer_beep_pattern(const char *deviceId, int onMs, int offMs, int count)
{
    if (!deviceId)
        return -1;

    return DeviceService::instance().buzzerBeepPattern(deviceId, onMs, offMs, count) ? 0 : -1;
}

int bridge_dht11_add(const char *deviceId, const char *devPath)
{
    if (!deviceId || !devPath)
        return -1;

    return DeviceService::instance().addDht11(deviceId, devPath) ? 0 : -1;
}

int bridge_dht11_get_temp(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getTemperature(deviceId);
}

int bridge_dht11_get_humi(const char *deviceId)
{
    if (!deviceId)
        return 0;

    return DeviceService::instance().getHumidity(deviceId);
}

void bridge_sensor_report_enable(int enable, int intervalSec)
{
    DeviceService::instance().enableSensorReport(enable != 0, intervalSec);
}

void bridge_sensor_report_once(void)
{
    DeviceService::instance().reportSensorData();
}

int bridge_mqtt_connect(const char *host, int port, const char *clientId)
{
    if (!host)
        return -1;

    std::string id = clientId ? clientId : "lvgl_device";
    return DeviceService::instance().connectMqtt(host, port, id) ? 0 : -1;
}

void bridge_mqtt_disconnect(void)
{
    DeviceService::instance().disconnectMqtt();
}

int bridge_mqtt_is_connected(void)
{
    return DeviceService::instance().isMqttConnected() ? 1 : 0;
}
