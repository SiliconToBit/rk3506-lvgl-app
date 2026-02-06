#include "app_bridge.h"
#include "Backlight.h"
#include "Dht11.h"
#include "MusicPlayer.h"
#include "WeatherService.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// 全局单例或静态实例
static MusicPlayer player;
static Dht11 dht11("/dev/dht11"); // 假设设备节点是 /dev/dht11
static Backlight backlight("/sys/class/backlight/backlight/xxxx");
// [重要] 请替换为你的 和风天气 (QWeather) API Key
// 可以在 https://console.qweather.com/ 创建项目获取 Key
static WeatherService weatherService("4661897787bc49c793207316caf28304");

static char *music_dir = "/root/Music"; // 默认音乐目录

// --- 音乐控制 ---

void bridge_music_scan_dir(const char *path)
{
    std::cout << "[Bridge] Scan music directory: " << path << std::endl;
    player.scanDirectory(path);

    // 扫描完成后，默认预加载第一首歌（如果存在）
    if (!player.getPlaylist().empty())
    {
        player.loadMusic(0);
    }
}

char **bridge_get_music_playlist(size_t *out_count)
{
    bridge_music_scan_dir(music_dir);

    std::vector<std::string> playlist = player.getPlaylist();
    *out_count = playlist.size();

    if (playlist.empty())

        return nullptr;

    // 分配字符串数组
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
    std::cout << "[Bridge] Play music" << std::endl;
    player.play(); // 示例文件
}

void bridge_music_pause(void)
{
    std::cout << "[Bridge] Pause music" << std::endl;
    player.pause();
}

void bridge_music_prev(void)
{
    std::cout << "[Bridge] Prev music" << std::endl;
    player.prev();
}

void bridge_music_next(void)
{
    std::cout << "[Bridge] Next music" << std::endl;
    player.next();
}

double bridge_music_current_time(void)
{
    return player.getMusicCurrentTime();
}

double bridge_music_duration(void)
{
    return player.getMusicDuration();
}

char *bridge_current_music_name(void)
{
    std::string music_name = player.getCurrentSongName();
    char *c_str = (char *)std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

char *bridge_current_song_lyrics(void)
{
    std::string lyrics = player.getCurrentSongLyrics();
    char *c_str = (char *)std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

char *bridge_get_current_lyric_line(void)
{
    double time = player.getMusicCurrentTime();
    std::string line = player.getCurrentLyricLine(time);
    char *c_str = (char *)std::malloc(line.size() + 1);
    std::strcpy(c_str, line.c_str());
    return c_str;
}

char *bridge_get_current_album_cover_path(void)
{
    std::string path = player.getCurrentAlbumCoverPath();
    if (path.empty())
        return nullptr;
    char *c_str = (char *)std::malloc(path.size() + 1);
    std::strcpy(c_str, path.c_str());
    return c_str;
}

// --- 天气控制 ---

void bridge_set_volume(long volume)
{
    // player.setVolume(volume);
}

// --- 天气控制 ---

void bridge_update_weather(void)
{
    std::cout << "[Bridge] Update weather (Default: 广州)" << std::endl;
    weatherService.updateWeatherAsync();
}

void bridge_update_weather_city(const char *city)
{
    if (city && strlen(city) > 0)
    {
        std::cout << "[Bridge] Update weather for city: " << city << std::endl;
        weatherService.updateWeatherAsync(city);
    }
}

void bridge_get_weather_data(AppWeatherData *data)
{
    if (!data)
        return;

    WeatherData cppData = weatherService.getWeatherData();

    // 复制基础数据
    strncpy(data->city, cppData.city.c_str(), sizeof(data->city) - 1);
    data->city[sizeof(data->city) - 1] = '\0';

    strncpy(data->weather, cppData.weather.c_str(), sizeof(data->weather) - 1);
    data->weather[sizeof(data->weather) - 1] = '\0';

    data->iconCode = cppData.iconCode;
    data->temperature = cppData.temperature;
    data->humidity = cppData.humidity;
    data->feelsLike = cppData.feelsLike;

    strncpy(data->windDir, cppData.windDir.c_str(), sizeof(data->windDir) - 1);
    data->windDir[sizeof(data->windDir) - 1] = '\0';

    data->windSpeed = cppData.windSpeed;

    // 复制预报数据 Day 1
    data->day1_iconCode = cppData.day1_iconCode;
    data->day1_tempMin = cppData.day1_tempMin;
    data->day1_tempMax = cppData.day1_tempMax;
    strncpy(data->day1_windDir, cppData.day1_windDir.c_str(), sizeof(data->day1_windDir) - 1);
    data->day1_windDir[sizeof(data->day1_windDir) - 1] = '\0';
    strncpy(data->day1_fxDate, cppData.day1_fxDate.c_str(), sizeof(data->day1_fxDate) - 1);
    data->day1_fxDate[sizeof(data->day1_fxDate) - 1] = '\0';

    // 复制预报数据 Day 2
    data->day2_iconCode = cppData.day2_iconCode;
    data->day2_tempMin = cppData.day2_tempMin;
    data->day2_tempMax = cppData.day2_tempMax;
    strncpy(data->day2_windDir, cppData.day2_windDir.c_str(), sizeof(data->day2_windDir) - 1);
    data->day2_windDir[sizeof(data->day2_windDir) - 1] = '\0';

    // 复制预报数据 Day 3
    data->day3_iconCode = cppData.day3_iconCode;
    data->day3_tempMin = cppData.day3_tempMin;
    data->day3_tempMax = cppData.day3_tempMax;
    strncpy(data->day3_windDir, cppData.day3_windDir.c_str(), sizeof(data->day3_windDir) - 1);
    data->day3_windDir[sizeof(data->day3_windDir) - 1] = '\0';
}

// --- 传感器数据 ---

// --- 获取天气数据 (Bridge 层负责将数值转为字符串供 LVGL 显示) ---

// --- 传感器数据 ---

int bridge_get_temp(void)
{
    // 尝试打开设备（如果还没打开）
    // 实际应用中可能在初始化时打开一次
    dht11.openDevice();
    return dht11.readTemperature();
}

int bridge_get_humi(void)
{
    dht11.openDevice();
    return dht11.readHumidity();
}

void bridge_set_brightness(int level)
{
}
