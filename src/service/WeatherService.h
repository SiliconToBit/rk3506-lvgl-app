/**
 * @file WeatherService.h
 * @brief 天气服务管理类
 * @details 负责获取和解析天气数据
 */

#ifndef LVGL_APP_SERVICE_WEATHER_SERVICE_H
#define LVGL_APP_SERVICE_WEATHER_SERVICE_H

#include <string>

struct WeatherData
{
    char city[64];
    char weather[64];
    int iconCode;
    int temperature;
    int humidity;
    int feelsLike;
    char windDir[64];
    int windSpeed;

    char day1_fxDate[16];
    int day1_tempMin;
    int day1_tempMax;
    char day1_windDir[64];
    int day1_iconCode;

    int day2_tempMin;
    int day2_tempMax;
    char day2_windDir[64];
    int day2_iconCode;

    int day3_tempMin;
    int day3_tempMax;
    char day3_windDir[64];
    int day3_iconCode;
};

class WeatherService
{
public:
    explicit WeatherService(const std::string& apiKey);
    ~WeatherService();

    void updateWeatherAsync(const std::string& city);
    WeatherData getWeatherData() const;

private:
    std::string m_apiKey;
    std::string m_baseUrl;
    WeatherData m_currentData;

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
    std::string performGetRequest(const std::string& url);
    std::string urlEncode(const std::string& value);
    std::string lookupLocationId(const std::string& cityName);
    void fetchFromApi(const std::string& city);
    void parseCurrentWeather(const std::string& response);
    void parseForecast(const std::string& response);
};

#endif // LVGL_APP_SERVICE_WEATHER_SERVICE_H
