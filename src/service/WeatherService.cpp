#include "WeatherService.h"
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <unistd.h>

using json = nlohmann::json;

/**
 * @brief 构造函数
 * @param apiKey 和风天气API密钥
 * @details 初始化API密钥、基础URL和cURL库
 */
WeatherService::WeatherService(const std::string &apiKey)
    : m_apiKey(apiKey), m_baseUrl("https://pu3qqpnwdn.re.qweatherapi.com")
{
    memset(&m_currentData, 0, sizeof(WeatherData));
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

/**
 * @brief 析构函数
 * @details 清理cURL全局资源
 */
WeatherService::~WeatherService()
{
    curl_global_cleanup();
}

/**
 * @brief cURL写回调函数
 * @param contents 接收到的数据
 * @param size 每个元素大小
 * @param nmemb 元素数量
 * @param userp 用户数据指针(字符串缓冲区)
 * @return size_t 已处理字节数
 */
size_t WeatherService::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief 执行HTTP GET请求
 * @param url 请求URL
 * @return std::string 响应内容
 * @details 使用cURL执行HTTPS请求,禁用SSL证书验证
 */
std::string WeatherService::performGetRequest(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
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

/**
 * @brief URL编码
 * @param value 待编码字符串
 * @return std::string 编码后的字符串
 */
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

/**
 * @brief 查询城市位置ID
 * @param cityName 城市名称或ID
 * @return std::string 城市ID,失败返回空字符串
 * @details 如果输入已是纯数字则直接返回,否则调用GeoAPI查询
 */
std::string WeatherService::lookupLocationId(const std::string &cityName)
{
    if (cityName.find_first_not_of("0123456789") == std::string::npos)
    {
        return cityName;
    }

    std::string encodedCity = urlEncode(cityName);
    std::string url = m_baseUrl + "/geo/v2/city/lookup?location=" + encodedCity + "&key=" + m_apiKey;
    std::cout << "Looking up Location ID for: " << cityName << " (Encoded: " << encodedCity << ")..." << std::endl;

    std::string response = performGetRequest(url);
    if (response.empty())
        return "";

    try
    {
        auto jsonResponse = json::parse(response);

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

/**
 * @brief 异步更新天气数据
 * @param city 城市名称
 * @details 启动后台线程获取天气数据
 */
void WeatherService::updateWeatherAsync(const std::string &city)
{
    std::thread t([this, city]() { this->fetchFromApi(city); });
    t.detach();
}

/**
 * @brief 从API获取天气数据
 * @param city 城市名称
 * @details 先查询城市ID,再获取当前天气和3天预报
 */
void WeatherService::fetchFromApi(const std::string &city)
{
    std::string locationId = lookupLocationId(city);
    if (locationId.empty())
    {
        std::cerr << "Invalid location or lookup failed for: " << city << std::endl;
        return;
    }

    std::string weatherUrl = m_baseUrl + "/v7/weather/now?location=" + locationId + "&key=" + m_apiKey;
    std::cout << "Fetching current weather for " << locationId << "..." << std::endl;
    std::string weatherResp = performGetRequest(weatherUrl);
    parseCurrentWeather(weatherResp);

    std::string forecastUrl = m_baseUrl + "/v7/weather/3d?location=" + locationId + "&key=" + m_apiKey;
    std::cout << "Fetching forecast for " << locationId << "..." << std::endl;
    std::string forecastResp = performGetRequest(forecastUrl);
    parseForecast(forecastResp);
}

/**
 * @brief 解析当前天气JSON响应
 * @param response JSON响应字符串
 * @details 解析温度、湿度、体感温度、风速、风向、天气图标等
 */
void WeatherService::parseCurrentWeather(const std::string &response)
{
    if (response.empty())
        return;

    try
    {
        auto jsonResponse = json::parse(response);

        if (jsonResponse.contains("error"))
        {
            std::cerr << "Weather API Error: " << jsonResponse["error"].dump() << std::endl;
            return;
        }

        std::string code = jsonResponse.value("code", "");

        if (code == "200" && jsonResponse.contains("now"))
        {
            auto now = jsonResponse["now"];
            try
            {
                m_currentData.temperature = std::stoi(now.value("temp", "0"));
                m_currentData.humidity = std::stoi(now.value("humidity", "0"));
                m_currentData.feelsLike = std::stoi(now.value("feelsLike", "0"));
                m_currentData.windSpeed = std::stoi(now.value("windSpeed", "0"));
                m_currentData.iconCode = std::stoi(now.value("icon", "0"));

                std::string windDir = now.value("windDir", "Unknown");
                strncpy(m_currentData.windDir, windDir.c_str(), sizeof(m_currentData.windDir) - 1);
                m_currentData.windDir[sizeof(m_currentData.windDir) - 1] = '\0';

                std::string weather = now.value("text", "Unknown");
                strncpy(m_currentData.weather, weather.c_str(), sizeof(m_currentData.weather) - 1);
                m_currentData.weather[sizeof(m_currentData.weather) - 1] = '\0';
            }
            catch (...)
            {
                m_currentData.temperature = 0;
                m_currentData.humidity = 0;
            }
        }
        else
        {
            std::cerr << "Weather API error code: " << code << std::endl;
            std::cerr << "Response: " << response << std::endl;
        }

        std::cout << "Current Weather: " << m_currentData.temperature << "C, " << m_currentData.weather << std::endl;
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error (Current): " << e.what() << std::endl;
        std::cerr << "Response content: " << response << std::endl;
    }
}

/**
 * @brief 解析天气预报JSON响应
 * @param response JSON响应字符串
 * @details 解析未来3天的天气预报数据
 */
void WeatherService::parseForecast(const std::string &response)
{
    if (response.empty())
        return;

    try
    {
        auto jsonResponse = json::parse(response);

        if (jsonResponse.contains("error"))
        {
            std::cerr << "Forecast API Error: " << jsonResponse["error"].dump() << std::endl;
            return;
        }

        std::string code = jsonResponse.value("code", "");

        if (code == "200" && jsonResponse.contains("daily") && !jsonResponse["daily"].empty())
        {
            auto firstDay = jsonResponse["daily"][0];
            std::string fxDate = firstDay.value("fxDate", "");
            strncpy(m_currentData.day1_fxDate, fxDate.c_str(), sizeof(m_currentData.day1_fxDate) - 1);
            m_currentData.day1_fxDate[sizeof(m_currentData.day1_fxDate) - 1] = '\0';

            m_currentData.day1_tempMin = std::stoi(firstDay.value("tempMin", "0"));
            m_currentData.day1_tempMax = std::stoi(firstDay.value("tempMax", "0"));
            m_currentData.day1_iconCode = std::stoi(firstDay.value("iconDay", "0"));

            std::string windDir = firstDay.value("windDirDay", "");
            strncpy(m_currentData.day1_windDir, windDir.c_str(), sizeof(m_currentData.day1_windDir) - 1);
            m_currentData.day1_windDir[sizeof(m_currentData.day1_windDir) - 1] = '\0';

            auto secondDay = jsonResponse["daily"][1];
            m_currentData.day2_tempMin = std::stoi(secondDay.value("tempMin", "0"));
            m_currentData.day2_tempMax = std::stoi(secondDay.value("tempMax", "0"));
            m_currentData.day2_iconCode = std::stoi(secondDay.value("iconDay", "0"));

            windDir = secondDay.value("windDirDay", "");
            strncpy(m_currentData.day2_windDir, windDir.c_str(), sizeof(m_currentData.day2_windDir) - 1);
            m_currentData.day2_windDir[sizeof(m_currentData.day2_windDir) - 1] = '\0';

            auto thirdDay = jsonResponse["daily"][2];
            m_currentData.day3_tempMin = std::stoi(thirdDay.value("tempMin", "0"));
            m_currentData.day3_tempMax = std::stoi(thirdDay.value("tempMax", "0"));
            m_currentData.day3_iconCode = std::stoi(thirdDay.value("iconDay", "0"));

            windDir = thirdDay.value("windDirDay", "");
            strncpy(m_currentData.day3_windDir, windDir.c_str(), sizeof(m_currentData.day3_windDir) - 1);
            m_currentData.day3_windDir[sizeof(m_currentData.day3_windDir) - 1] = '\0';
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

/**
 * @brief 获取天气数据
 * @return WeatherData 当前天气数据结构
 */
WeatherData WeatherService::getWeatherData() const
{
    return m_currentData;
}
