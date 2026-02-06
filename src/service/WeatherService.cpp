#include "WeatherService.h"
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <unistd.h>

using json = nlohmann::json;

WeatherService::WeatherService(const std::string &apiKey)
    : apiKey(apiKey), baseUrl("https://pu3qqpnwdn.re.qweatherapi.com")
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

WeatherService::~WeatherService()
{
    curl_global_cleanup();
}

size_t WeatherService::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

std::string WeatherService::performGetRequest(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        // 忽略 SSL 证书验证 (仅用于测试，生产环境建议配置 CA 证书)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // 【关键】设置 Accept-Encoding 为空字符串，让 libcurl 自动处理所有支持的压缩格式 (gzip, deflate 等)
        // 并自动解压响应数据
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

std::string WeatherService::urlEncode(const std::string &value)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        char *output = curl_easy_escape(curl, value.c_str(), value.length());
        if (output)
        {
            std::string result(output);
            curl_free(output);
            curl_easy_cleanup(curl);
            return result;
        }
        curl_easy_cleanup(curl);
    }
    return value;
}

std::string WeatherService::lookupLocationId(const std::string &cityName)
{
    // 简单的判断：如果已经是纯数字，直接返回（假设是 Location ID）
    if (cityName.find_first_not_of("0123456789") == std::string::npos)
    {
        return cityName;
    }

    // GeoAPI URL (注意：使用 geoapi.qweather.com)
    std::string encodedCity = urlEncode(cityName);
    std::string url = baseUrl + "/geo/v2/city/lookup?location=" + encodedCity + "&key=" + apiKey;
    std::cout << "Looking up Location ID for: " << cityName << " (Encoded: " << encodedCity << ")..." << std::endl;

    std::string response = performGetRequest(url);
    if (response.empty())
        return "";

    try
    {
        auto jsonResponse = json::parse(response);

        // 检查是否有 API 错误信息 (如 401 Unauthorized)
        if (jsonResponse.contains("error"))
        {
            std::cerr << "GeoAPI Error: " << jsonResponse["error"].dump() << std::endl;
            std::cerr << "Please check your API Key in app_bridge.cpp!" << std::endl;
            return "";
        }

        std::string code = jsonResponse.value("code", "");

        if (code == "200" && jsonResponse.contains("location") && !jsonResponse["location"].empty())
        {
            std::string id = jsonResponse["location"][0].value("id", "");
            std::string name = jsonResponse["location"][0].value("name", "");
            std::cout << "Found Location ID: " << id << " (" << name << ")" << std::endl;
            return id;
        }
        else
        {
            std::cerr << "GeoAPI lookup failed. Code: " << code << std::endl;
            std::cerr << "Response: " << response << std::endl;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error (GeoAPI): " << e.what() << std::endl;
        std::cerr << "Response content: " << response << std::endl;
    }
    return "";
}

void WeatherService::updateWeatherAsync(const std::string &city)
{
    // 启动一个线程来模拟网络请求，避免阻塞 UI
    std::thread t([this, city]() { this->fetchFromApi(city); });
    t.detach();
}

void WeatherService::fetchFromApi(const std::string &city)
{
    // 1. 获取 Location ID
    std::string locationId = lookupLocationId(city);
    if (locationId.empty())
    {
        std::cerr << "Invalid location or lookup failed for: " << city << std::endl;
        return;
    }

    // 2. 获取当前天气 (和风天气 API)
    // 注意：city 参数应为 Location ID (如 101010100) 或经纬度
    std::string weatherUrl = baseUrl + "/v7/weather/now?location=" + locationId + "&key=" + apiKey;
    std::cout << "Fetching current weather for " << locationId << "..." << std::endl;
    std::string weatherResp = performGetRequest(weatherUrl);
    parseCurrentWeather(weatherResp);

    // 3. 获取天气预报 (3天预报)
    std::string forecastUrl = baseUrl + "/v7/weather/3d?location=" + locationId + "&key=" + apiKey;
    std::cout << "Fetching forecast for " << locationId << "..." << std::endl;
    std::string forecastResp = performGetRequest(forecastUrl);
    parseForecast(forecastResp);
}

void WeatherService::parseCurrentWeather(const std::string &response)
{
    if (response.empty())
        return;

    try
    {
        auto jsonResponse = json::parse(response);

        // 检查是否有 API 错误信息
        if (jsonResponse.contains("error"))
        {
            std::cerr << "Weather API Error: " << jsonResponse["error"].dump() << std::endl;
            return;
        }

        // 和风天气 JSON 结构:
        // { "code": "200", "now": { "temp": "24", "text": "多云", "humidity": "72", ... } }
        std::string code = jsonResponse.value("code", "");

        if (code == "200" && jsonResponse.contains("now"))
        {
            auto now = jsonResponse["now"];
            // 和风返回的数值通常是字符串，需要转换
            try
            {
                currentData.temperature = std::stoi(now.value("temp", "0"));
                currentData.humidity = std::stoi(now.value("humidity", "0"));
                currentData.feelsLike = std::stoi(now.value("feelsLike", "0"));
                currentData.windDir = now.value("windDir", "0");
                currentData.windSpeed = std::stoi(now.value("windSpeed", "0"));
                currentData.iconCode = std::stoi(now.value("icon", "0"));
                currentData.windSpeed = std::stoi(now.value("windSpeed", "0"));
            }
            catch (...)
            {
                currentData.temperature = 0.0f;
                currentData.humidity = 0.0f;
            }
            currentData.weather = now.value("text", "Unknown");
            // 和风实时天气接口不返回城市名，这里暂时保留原值或设为未知
            // currentData.city = ...;
        }
        else
        {
            std::cerr << "Weather API error code: " << code << std::endl;
            std::cerr << "Response: " << response << std::endl;
        }

        std::cout << "Current Weather: " << currentData.temperature << "C, " << currentData.weather << std::endl;
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error (Current): " << e.what() << std::endl;
        std::cerr << "Response content: " << response << std::endl;
    }
}

void WeatherService::parseForecast(const std::string &response)
{
    if (response.empty())
        return;

    try
    {
        auto jsonResponse = json::parse(response);

        // 检查是否有 API 错误信息
        if (jsonResponse.contains("error"))
        {
            std::cerr << "Forecast API Error: " << jsonResponse["error"].dump() << std::endl;
            return;
        }

        // 和风天气预报结构: { "code": "200", "daily": [ { "fxDate": "...", "tempMax": "...", ... } ] }
        std::string code = jsonResponse.value("code", "");

        if (code == "200" && jsonResponse.contains("daily") && !jsonResponse["daily"].empty())
        {
            auto firstDay = jsonResponse["daily"][0];
            currentData.day1_fxDate = firstDay.value("fxDate", "");
            currentData.day1_tempMin = std::stoi(firstDay.value("tempMin", "0"));
            currentData.day1_tempMax = std::stoi(firstDay.value("tempMax", "0"));
            currentData.day1_windDir = firstDay.value("windDirDay", "");
            currentData.day2_iconCode = std::stoi(firstDay.value("iconDay", "0"));

            auto secondDay = jsonResponse["daily"][1];
            currentData.day2_tempMin = std::stoi(secondDay.value("tempMin", "0"));
            currentData.day2_tempMax = std::stoi(secondDay.value("tempMax", "0"));
            currentData.day2_windDir = secondDay.value("windDirDay", "");
            currentData.day2_iconCode = std::stoi(secondDay.value("iconDay", "0"));

            auto thirdDay = jsonResponse["daily"][2];
            currentData.day3_tempMin = std::stoi(thirdDay.value("tempMin", "0"));
            currentData.day3_tempMax = std::stoi(thirdDay.value("tempMax", "0"));
            currentData.day3_windDir = thirdDay.value("windDirDay", "");
            currentData.day3_iconCode = std::stoi(thirdDay.value("iconDay", "0"));
        }
        else
        {
            std::cerr << "Forecast API error code: " << code << std::endl;
            std::cerr << "Response: " << response << std::endl;
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error (Forecast): " << e.what() << std::endl;
        std::cerr << "Response content: " << response << std::endl;
    }
}

WeatherData WeatherService::getWeatherData() const
{
    return currentData;
}
