/**
 * @file TimeService.h
 * @brief 时间服务类
 * @details 提供网络时间同步、本地时间获取功能
 */

#ifndef LVGL_APP_SERVICE_TIME_SERVICE_H
#define LVGL_APP_SERVICE_TIME_SERVICE_H

#include <string>
#include <functional>

/**
 * @brief 日期时间结构
 */
struct DateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int weekday; // 0=周日, 1=周一, ..., 6=周六
};

/**
 * @brief 时间服务类
 * @details 单例模式，提供网络时间同步
 */
class TimeService
{
public:
    /**
     * @brief 获取单例实例
     * @return TimeService& 单例引用
     */
    static TimeService& getInstance();

    TimeService(const TimeService&) = delete;
    TimeService& operator=(const TimeService&) = delete;

    /**
     * @brief 析构函数
     */
    ~TimeService();

    /**
     * @brief 初始化时间服务
     * @param ntpServer NTP服务器地址
     */
    void init(const std::string& ntpServer = "ntp.aliyun.com");

    /**
     * @brief 同步网络时间
     * @return bool 同步是否成功
     */
    bool syncTime();

    /**
     * @brief 获取当前日期时间
     * @return DateTime 当前日期时间
     */
    DateTime getCurrentDateTime() const;

    /**
     * @brief 获取当前时间字符串（HH:MM）
     * @return std::string 时间字符串
     */
    std::string getTimeString() const;

    /**
     * @brief 获取当前日期字符串（YYYY-MM-DD）
     * @return std::string 日期字符串
     */
    std::string getDateString() const;

    /**
     * @brief 获取当前完整日期字符串（YYYY-MM-DD 星期X）
     * @return std::string 完整日期字符串
     */
    std::string getFullDateString() const;

    /**
     * @brief 获取星期字符串
     * @param weekday 星期索引（0-6）
     * @return const char* 星期字符串
     */
    static const char* getWeekdayString(int weekday);

private:
    std::string m_ntpServer;
    bool m_initialized;

    TimeService();
};

#endif // LVGL_APP_SERVICE_TIME_SERVICE_H
