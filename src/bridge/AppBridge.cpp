#include "AppBridge.h"
#include "Backlight.h"
#include "Dht11.h"
#include "MusicPlayer.h"
#include "WeatherService.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static MusicPlayer s_player;
static Dht11 s_dht11("/dev/dht11");
static Backlight s_backlight("/sys/class/backlight/backlight/xxxx");
static WeatherService s_weatherService("4661897787bc49c793207316caf28304");

static char* s_musicDir = "/root/Music";

void bridge_music_scan_dir(const char* path)
{
    std::cout << "[Bridge] Scan music directory: " << path << std::endl;
    s_player.scanDirectory(path);

    if (!s_player.getPlaylist().empty())
    {
        s_player.loadMusic(0);
    }
}

char** bridge_get_music_playlist(size_t* out_count)
{
    bridge_music_scan_dir(s_musicDir);

    std::vector<std::string> playlist = s_player.getPlaylist();
    *out_count = playlist.size();

    if (playlist.empty())
        return nullptr;

    char** c_array = (char**)std::malloc(playlist.size() * sizeof(char*));
    for (size_t i = 0; i < playlist.size(); ++i)
    {
        c_array[i] = (char*)std::malloc(playlist[i].size() + 1);
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
    s_player.play();
}

void bridge_music_pause(void)
{
    std::cout << "[Bridge] Pause music" << std::endl;
    s_player.pause();
}

void bridge_music_prev(void)
{
    std::cout << "[Bridge] Prev music" << std::endl;
    s_player.prev();
}

void bridge_music_next(void)
{
    std::cout << "[Bridge] Next music" << std::endl;
    s_player.next();
}

double bridge_music_current_time(void)
{
    return s_player.getMusicCurrentTime();
}

double bridge_music_duration(void)
{
    return s_player.getMusicDuration();
}

char* bridge_current_music_name(void)
{
    std::string music_name = s_player.getCurrentSongName();
    char* c_str = (char*)std::malloc(music_name.size() + 1);
    std::strcpy(c_str, music_name.c_str());
    return c_str;
}

char* bridge_current_song_lyrics(void)
{
    std::string lyrics = s_player.getCurrentSongLyrics();
    char* c_str = (char*)std::malloc(lyrics.size() + 1);
    std::strcpy(c_str, lyrics.c_str());
    return c_str;
}

char* bridge_get_current_lyric_line(void)
{
    double time = s_player.getMusicCurrentTime();
    std::string line = s_player.getCurrentLyricLine(time);
    char* c_str = (char*)std::malloc(line.size() + 1);
    std::strcpy(c_str, line.c_str());
    return c_str;
}

char* bridge_get_current_album_cover_path(void)
{
    std::string path = s_player.getCurrentAlbumCoverPath();
    if (path.empty())
        return nullptr;
    char* c_str = (char*)std::malloc(path.size() + 1);
    std::strcpy(c_str, path.c_str());
    return c_str;
}

void bridge_set_volume(long volume)
{
    s_player.setVolume(volume);
}

void bridge_update_weather(void)
{
    std::cout << "[Bridge] Update weather (Default: 广州)" << std::endl;
    s_weatherService.updateWeatherAsync("广州");
}

void bridge_update_weather_city(const char* city)
{
    if (city && strlen(city) > 0)
    {
        std::cout << "[Bridge] Update weather for city: " << city << std::endl;
        s_weatherService.updateWeatherAsync(city);
    }
}

void bridge_get_weather_data(AppWeatherData* data)
{
    if (!data)
        return;

    WeatherData cppData = s_weatherService.getWeatherData();

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
    s_dht11.open();
    return s_dht11.readTemperature();
}

int bridge_get_humi(void)
{
    s_dht11.open();
    return s_dht11.readHumidity();
}

void bridge_set_brightness(int level)
{
    s_backlight.setBrightness(level);
}
