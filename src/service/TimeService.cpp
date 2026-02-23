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

TimeService::TimeService() : m_ntpServer("ntp.aliyun.com"), m_initialized(false) {}

TimeService::~TimeService() {}

void TimeService::init(const std::string& ntpServer)
{
    m_ntpServer = ntpServer;
    m_initialized = true;
}

bool TimeService::syncTime()
{
    // 使用 chronyc 同步时间
    std::string cmd = "chronyc -a makestep > /dev/null 2>&1";
    int ret = system(cmd.c_str());

    if (ret == 0)
    {
        return true;
    }
    else
    {
        // 如果 chronyc 失败，尝试使用 ntpdate
        cmd = "ntpdate -u " + m_ntpServer + " > /dev/null 2>&1";
        ret = system(cmd.c_str());
        if (ret == 0)
        {
            return true;
        }
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
