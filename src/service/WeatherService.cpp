/**
 * @file WeatherService.cpp
 * @brief 天气服务类实现
 */

#include "WeatherService.h"
#include "../AppConfig.h"
#include "../fonts/qweather_icons.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>

// 使用 AppConfig.h 中的API配置

WeatherService& WeatherService::getInstance()
{
    static WeatherService instance;
    return instance;
}

WeatherService::WeatherService() : m_cityCode("101250412"), m_cityName("衡阳/常宁"), m_valid(false), m_lastUpdate(0) {}

WeatherService::~WeatherService() {}

void WeatherService::init(const std::string& city)
{
    m_cityName = city;
    m_valid = false;
    m_lastUpdate = 0;
}

void WeatherService::setCityCode(const std::string& cityCode)
{
    m_cityCode = cityCode;
    m_valid = false;
}

bool WeatherService::update()
{
    auto now = std::chrono::system_clock::now();
    auto now_sec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    if (m_valid && (now_sec - m_lastUpdate) < UPDATE_INTERVAL)
    {
        return true;
    }

    if (!fetchData())
    {
        return false;
    }

    m_lastUpdate = now_sec;
    m_valid = true;

    return true;
}

void WeatherService::updateAsync(const std::string& city)
{
    if (!city.empty() && m_cityName != city)
    {
        m_cityName = city;
        m_valid = false;
    }
    update();
}

CurrentWeather WeatherService::getCurrent() const
{
    return m_current;
}

std::vector<ForecastWeather> WeatherService::getForecast() const
{
    return m_forecast;
}

bool WeatherService::isValid() const
{
    return m_valid;
}

void WeatherService::setCity(const std::string& city)
{
    if (m_cityName != city)
    {
        m_cityName = city;
        m_valid = false;
    }
}

std::string WeatherService::getCity() const
{
    return m_cityName;
}

std::string WeatherService::getCityCode() const
{
    return m_cityCode;
}

// HTTP请求回调函数
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
    userp->append((char*) contents, size * nmemb);
    return size * nmemb;
}

// 执行HTTP GET请求
static bool httpGet(const std::string& url, std::string& response)
{
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        return false;
    }

    return httpCode == 200;
}

// 解析JSON中的字符串值
static std::string parseJsonString(const std::string& json, const std::string& key)
{
    size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos)
        return "";

    pos = json.find("\"", pos + key.length() + 2);
    if (pos == std::string::npos)
        return "";

    size_t end = json.find("\"", pos + 1);
    if (end == std::string::npos)
        return "";

    return json.substr(pos + 1, end - pos - 1);
}

// 解析JSON中的整数值
static int parseJsonInt(const std::string& json, const std::string& key)
{
    std::string value = parseJsonString(json, key);
    if (value.empty())
    {
        // 尝试解析数字格式
        size_t pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos)
            return 0;

        pos = json.find(":", pos);
        if (pos == std::string::npos)
            return 0;

        // 跳过空格
        while (pos < json.length() && (json[pos] == ':' || json[pos] == ' ' || json[pos] == '\t'))
            pos++;

        size_t end = pos;
        while (end < json.length() && (json[end] == '-' || (json[end] >= '0' && json[end] <= '9')))
            end++;

        try
        {
            return std::stoi(json.substr(pos, end - pos));
        }
        catch (...)
        {
            return 0;
        }
    }
    try
    {
        return std::stoi(value);
    }
    catch (...)
    {
        return 0;
    }
}

bool WeatherService::fetchData()
{
    // 构建实时天气API URL，使用宏定义
    char currentUrl[256];
    snprintf(currentUrl, sizeof(currentUrl), APP_WEATHER_NOW_API_URL, m_cityCode.c_str());

    std::string currentResponse;
    if (!httpGet(currentUrl, currentResponse))
    {
        return false;
    }

    // 解析实时天气
    std::string code = parseJsonString(currentResponse, "code");
    if (code != "200")
    {
        return false;
    }

    // 解析now字段中的数据
    size_t nowPos = currentResponse.find("\"now\"");
    if (nowPos == std::string::npos)
    {
        return false;
    }

    std::string nowSection = currentResponse.substr(nowPos, 500);

    m_current.city = m_cityName;
    m_current.temperature = parseJsonInt(nowSection, "temp");
    m_current.feelsLike = parseJsonInt(nowSection, "feelsLike");
    m_current.humidity = parseJsonInt(nowSection, "humidity");
    m_current.iconCode = parseJsonInt(nowSection, "icon");
    m_current.description = parseJsonString(nowSection, "text");
    m_current.windDir = parseJsonString(nowSection, "windDir");
    m_current.windSpeed = parseJsonInt(nowSection, "windScale");

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M");
    m_current.updateTime = ss.str();

    // 构建3天预报API URL，使用宏定义
    char forecastUrl[256];
    snprintf(forecastUrl, sizeof(forecastUrl), APP_WEATHER_3D_API_URL, m_cityCode.c_str());

    std::string forecastResponse;
    if (!httpGet(forecastUrl, forecastResponse))
    {
        return true; // 实时天气获取成功，预报失败也算部分成功
    }

    // 解析预报数据 - 从daily数组中解析
    m_forecast.clear();

    const char* dayNames[] = {"今天", "明天", "后天"};

    // 查找daily数组
    size_t dailyPos = forecastResponse.find("\"daily\"");
    if (dailyPos == std::string::npos)
    {
        return true; // 没有daily字段，但实时天气已成功
    }

    // 提取daily数组内容
    size_t arrayStart = forecastResponse.find("[", dailyPos);
    size_t arrayEnd = forecastResponse.find("]", arrayStart);
    if (arrayStart == std::string::npos || arrayEnd == std::string::npos)
    {
        return true;
    }

    std::string dailyArray = forecastResponse.substr(arrayStart, arrayEnd - arrayStart + 1);

    // 解析每一天的数据
    size_t pos = 0;
    int dayCount = 0;
    while (pos < dailyArray.length() && dayCount < 3)
    {
        size_t objStart = dailyArray.find("{", pos);
        if (objStart == std::string::npos)
            break;

        size_t objEnd = dailyArray.find("}", objStart);
        if (objEnd == std::string::npos)
            break;

        std::string dayObj = dailyArray.substr(objStart, objEnd - objStart + 1);

        ForecastWeather fc;
        fc.date = dayNames[dayCount];
        fc.iconCode = parseJsonInt(dayObj, "iconDay");
        fc.tempMin = parseJsonInt(dayObj, "tempMin");
        fc.tempMax = parseJsonInt(dayObj, "tempMax");
        fc.windDir = parseJsonString(dayObj, "windDirDay");

        m_forecast.push_back(fc);
        dayCount++;
        pos = objEnd + 1;
    }

    return true;
}

const char* WeatherService::getIconStr(int code)
{
    switch (code)
    {
        case 100:
            return QW_ICON_100;
        case 101:
            return QW_ICON_101;
        case 102:
            return QW_ICON_102;
        case 103:
            return QW_ICON_103;
        case 104:
            return QW_ICON_104;
        case 150:
            return QW_ICON_150;
        case 151:
            return QW_ICON_151;
        case 152:
            return QW_ICON_152;
        case 153:
            return QW_ICON_153;
        case 300:
            return QW_ICON_300;
        case 301:
            return QW_ICON_301;
        case 302:
            return QW_ICON_302;
        case 303:
            return QW_ICON_303;
        case 304:
            return QW_ICON_304;
        case 305:
            return QW_ICON_305;
        case 306:
            return QW_ICON_306;
        case 307:
            return QW_ICON_307;
        case 308:
            return QW_ICON_308;
        case 309:
            return QW_ICON_309;
        case 310:
            return QW_ICON_310;
        case 311:
            return QW_ICON_311;
        case 312:
            return QW_ICON_312;
        case 313:
            return QW_ICON_313;
        case 314:
            return QW_ICON_314;
        case 315:
            return QW_ICON_315;
        case 316:
            return QW_ICON_316;
        case 317:
            return QW_ICON_317;
        case 318:
            return QW_ICON_318;
        case 350:
            return QW_ICON_350;
        case 351:
            return QW_ICON_351;
        case 399:
            return QW_ICON_399;
        case 400:
            return QW_ICON_400;
        case 401:
            return QW_ICON_401;
        case 402:
            return QW_ICON_402;
        case 403:
            return QW_ICON_403;
        case 404:
            return QW_ICON_404;
        case 405:
            return QW_ICON_405;
        case 406:
            return QW_ICON_406;
        case 407:
            return QW_ICON_407;
        case 408:
            return QW_ICON_408;
        case 409:
            return QW_ICON_409;
        case 410:
            return QW_ICON_410;
        case 456:
            return QW_ICON_456;
        case 457:
            return QW_ICON_457;
        case 499:
            return QW_ICON_499;
        case 500:
            return QW_ICON_500;
        case 501:
            return QW_ICON_501;
        case 502:
            return QW_ICON_502;
        case 503:
            return QW_ICON_503;
        case 504:
            return QW_ICON_504;
        case 507:
            return QW_ICON_507;
        case 508:
            return QW_ICON_508;
        case 509:
            return QW_ICON_509;
        case 510:
            return QW_ICON_510;
        case 511:
            return QW_ICON_511;
        case 512:
            return QW_ICON_512;
        case 513:
            return QW_ICON_513;
        case 514:
            return QW_ICON_514;
        case 515:
            return QW_ICON_515;
        case 800:
            return QW_ICON_800;
        case 801:
            return QW_ICON_801;
        case 802:
            return QW_ICON_802;
        case 803:
            return QW_ICON_803;
        case 804:
            return QW_ICON_804;
        case 805:
            return QW_ICON_805;
        case 806:
            return QW_ICON_806;
        case 807:
            return QW_ICON_807;
        case 900:
            return QW_ICON_900;
        case 901:
            return QW_ICON_901;
        case 999:
            return QW_ICON_999;
        default:
            return QW_ICON_999;
    }
}
