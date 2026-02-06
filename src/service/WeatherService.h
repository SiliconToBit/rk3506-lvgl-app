#ifndef WEATHER_SERVICE_HPP
#define WEATHER_SERVICE_HPP

#include "nlohmann/json.hpp"
#include <functional>
#include <string>

// 天气数据结构
struct WeatherData
{
    std::string city;
    std::string weather;
    int iconCode;
    int temperature;
    int humidity;
    int feelsLike;
    std::string windDir;
    int windSpeed;

    int day1_iconCode;
    int day1_tempMin;
    int day1_tempMax;
    std::string day1_windDir;
    std::string day1_fxDate;

    int day2_iconCode;
    int day2_tempMin;
    int day2_tempMax;
    std::string day2_windDir;

    int day3_iconCode;
    int day3_tempMin;
    int day3_tempMax;
    std::string day3_windDir;
};

class WeatherService
{
  public:
    WeatherService(const std::string &apiKey = "4661897787bc49c793207316caf28304");
    ~WeatherService();

    // 异步更新天气
    void updateWeatherAsync(const std::string &city = "广州");

    // 获取当前天气数据
    WeatherData getWeatherData() const;

  private:
    std::string apiKey;
    const std::string baseUrl;
    WeatherData currentData;

    // Libcurl 的回调函数 (必须是 static)
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    // 内部帮助函数：发送 HTTP GET 请求
    std::string performGetRequest(const std::string &url);

    // URL 编码
    std::string urlEncode(const std::string &value);

    // GeoAPI 城市查找
    std::string lookupLocationId(const std::string &cityName);

    void fetchFromApi(const std::string &city);

    // 解析不同类型的响应
    void parseCurrentWeather(const std::string &payload);
    void parseForecast(const std::string &payload);
};

#endif // WEATHER_SERVICE_HPP
