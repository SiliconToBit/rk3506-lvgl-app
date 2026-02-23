/**
 * @file WeatherService.h
 * @brief 天气服务类
 * @details 提供天气数据获取、解析和缓存功能
 */

#ifndef LVGL_APP_SERVICE_WEATHER_SERVICE_H
#define LVGL_APP_SERVICE_WEATHER_SERVICE_H

#include <string>
#include <vector>

/**
 * @brief 当前天气数据
 */
struct CurrentWeather
{
    std::string city;        // 城市名称
    std::string description; // 天气描述（晴、多云等）
    int iconCode;            // 天气代码（用于图标）
    int temperature;         // 当前温度
    int humidity;            // 湿度百分比
    int feelsLike;           // 体感温度
    std::string windDir;     // 风向
    int windSpeed;           // 风速
    std::string updateTime;  // 更新时间
};

/**
 * @brief 天气预报数据
 */
struct ForecastWeather
{
    std::string date;    // 日期
    int iconCode;        // 天气代码
    int tempMin;         // 最低温度
    int tempMax;         // 最高温度
    std::string windDir; // 风向
};

/**
 * @brief 天气服务类
 * @details 单例模式，提供天气数据获取和管理
 */
class WeatherService
{
public:
    /**
     * @brief 获取单例实例
     * @return WeatherService& 单例引用
     */
    static WeatherService& getInstance();

    WeatherService(const WeatherService&) = delete;
    WeatherService& operator=(const WeatherService&) = delete;

    /**
     * @brief 析构函数
     */
    ~WeatherService();

    /**
     * @brief 初始化天气服务
     * @param city 默认城市名称
     */
    void init(const std::string& city = "北京");

    /**
     * @brief 更新天气数据
     * @return bool 更新是否成功
     */
    bool update();

    /**
     * @brief 异步更新天气数据
     * @param city 城市名称
     */
    void updateAsync(const std::string& city);

    /**
     * @brief 获取当前天气
     * @return CurrentWeather 当前天气数据
     */
    CurrentWeather getCurrent() const;

    /**
     * @brief 获取天气预报
     * @return std::vector<ForecastWeather> 未来几天预报
     */
    std::vector<ForecastWeather> getForecast() const;

    /**
     * @brief 检查数据是否有效
     * @return bool 数据是否有效
     */
    bool isValid() const;

    /**
     * @brief 设置当前城市
     * @param city 城市名称
     */
    void setCity(const std::string& city);

    /**
     * @brief 获取当前城市
     * @return std::string 城市名称
     */
    std::string getCity() const;

    /**
     * @brief 设置城市代码
     * @param cityCode 城市代码（如 101250412）
     */
    void setCityCode(const std::string& cityCode);

    /**
     * @brief 获取城市代码
     * @return std::string 城市代码
     */
    std::string getCityCode() const;

    /**
     * @brief 根据天气代码获取图标字符串
     * @param code 天气代码
     * @return const char* 图标字符串（如 "\uf101"）
     * @details 和风天气代码转图标字体字符串
     */
    static const char* getIconStr(int code);

private:
    CurrentWeather m_current;                // 当前天气
    std::vector<ForecastWeather> m_forecast; // 天气预报
    std::string m_cityName;                  // 当前城市名称
    std::string m_cityCode;                  // 当前城市代码
    bool m_valid = false;                    // 数据是否有效
    long long m_lastUpdate = 0;              // 上次更新时间戳

    static constexpr int UPDATE_INTERVAL = 600; // 更新间隔（秒，10分钟）

    WeatherService();

    bool fetchData();
};

#endif // LVGL_APP_SERVICE_WEATHER_SERVICE_H
