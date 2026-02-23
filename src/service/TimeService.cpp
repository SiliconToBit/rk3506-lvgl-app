/**
 * @file TimeService.cpp
 * @brief 时间服务类实现
 */

#include "TimeService.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

// 使用系统命令同步NTP时间
#include <cstdlib>

TimeService& TimeService::getInstance()
{
    static TimeService instance;
    return instance;
}

TimeService::TimeService() : m_ntpServer("ntp.aliyun.com"), m_initialized(false)
{
    std::cout << "[TimeService] Created" << std::endl;
}

TimeService::~TimeService()
{
    std::cout << "[TimeService] Destroyed" << std::endl;
}

void TimeService::init(const std::string& ntpServer)
{
    m_ntpServer = ntpServer;
    m_initialized = true;
    std::cout << "[TimeService] Initialized with NTP server: " << ntpServer << std::endl;
}

bool TimeService::syncTime()
{
    std::cout << "[TimeService] Syncing time from " << m_ntpServer << std::endl;

    // 使用 ntpdate 同步时间
    std::string cmd = "ntpdate -u " + m_ntpServer + " > /dev/null 2>&1";
    int ret = system(cmd.c_str());

    if (ret == 0)
    {
        std::cout << "[TimeService] Time synced successfully" << std::endl;
        return true;
    }
    else
    {
        std::cerr << "[TimeService] Failed to sync time, using local time" << std::endl;
        return false;
    }
}

DateTime TimeService::getCurrentDateTime() const
{
    DateTime dt = {0};

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    struct tm* local_time = std::localtime(&time_t_now);

    if (local_time)
    {
        dt.year = local_time->tm_year + 1900;
        dt.month = local_time->tm_mon + 1;
        dt.day = local_time->tm_mday;
        dt.hour = local_time->tm_hour;
        dt.minute = local_time->tm_min;
        dt.second = local_time->tm_sec;
        dt.weekday = local_time->tm_wday;
    }

    return dt;
}

std::string TimeService::getTimeString() const
{
    DateTime dt = getCurrentDateTime();
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << dt.hour << ":" << std::setw(2) << dt.minute;
    return ss.str();
}

std::string TimeService::getDateString() const
{
    DateTime dt = getCurrentDateTime();
    std::stringstream ss;
    ss << dt.year << "-" << std::setfill('0') << std::setw(2) << dt.month << "-" << std::setw(2) << dt.day;
    return ss.str();
}

const char* TimeService::getWeekdayString(int weekday)
{
    static const char* weekdays[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    if (weekday >= 0 && weekday <= 6)
    {
        return weekdays[weekday];
    }
    return "未知";
}

std::string TimeService::getFullDateString() const
{
    DateTime dt = getCurrentDateTime();
    std::stringstream ss;
    ss << dt.year << "-" << std::setfill('0') << std::setw(2) << dt.month << "-" << std::setw(2) << dt.day << " "
       << getWeekdayString(dt.weekday);
    return ss.str();
}
