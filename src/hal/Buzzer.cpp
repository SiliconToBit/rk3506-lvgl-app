#include "Buzzer.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief 构造函数
 * @param gpioPath GPIO设备路径,如 "/sys/class/gpio/gpio12/value"
 * @details 初始化蜂鸣器对象,设置初始状态为关闭未打开
 */
Buzzer::Buzzer(const std::string& gpioPath)
    : m_gpioPath(gpioPath)
    , m_fd(-1)
    , m_isOn(false)
    , m_isOpen(false)
{
}

/**
 * @brief 析构函数
 * @details 自动关闭蜂鸣器并释放GPIO资源
 */
Buzzer::~Buzzer()
{
    close();
}

/**
 * @brief 打开GPIO设备
 * @return true 打开成功
 * @return false 打开失败
 * @details 以只写方式打开GPIO设备文件,用于控制蜂鸣器
 */
bool Buzzer::open()
{
    m_fd = ::open(m_gpioPath.c_str(), O_WRONLY);
    if (m_fd < 0)
    {
        std::cerr << "[Buzzer] Failed to open " << m_gpioPath << std::endl;
        return false;
    }
    m_isOpen = true;
    return true;
}

/**
 * @brief 关闭GPIO设备
 * @details 先关闭蜂鸣器,再关闭文件描述符,重置状态标志
 */
void Buzzer::close()
{
    if (m_fd >= 0)
    {
        setOff();
        ::close(m_fd);
        m_fd = -1;
    }
    m_isOpen = false;
}

/**
 * @brief 写入GPIO值
 * @param value 要写入的值,0或1
 * @return true 写入成功
 * @return false 写入失败
 * @details 将0或1写入GPIO设备,控制蜂鸣器开关状态
 */
bool Buzzer::writeValue(int value)
{
    if (m_fd < 0)
    {
        return false;
    }

    const char* val = value ? "1" : "0";
    ssize_t ret = ::write(m_fd, val, 1);
    if (ret < 0)
    {
        std::cerr << "[Buzzer] Write failed" << std::endl;
        return false;
    }

    m_isOn = (value != 0);
    return true;
}

/**
 * @brief 打开蜂鸣器
 * @return true 操作成功
 * @return false 操作失败
 */
bool Buzzer::setOn()
{
    return writeValue(1);
}

/**
 * @brief 关闭蜂鸣器
 * @return true 操作成功
 * @return false 操作失败
 */
bool Buzzer::setOff()
{
    return writeValue(0);
}

/**
 * @brief 蜂鸣器响一声
 * @param durationMs 响铃持续时间(毫秒)
 * @return true 操作成功
 * @return false 操作失败
 * @details 打开蜂鸣器,等待指定时间后关闭
 */
bool Buzzer::beep(int durationMs)
{
    if (!setOn())
    {
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));

    return setOff();
}

/**
 * @brief 蜂鸣器响多声
 * @param onMs 每声持续时间(毫秒)
 * @param offMs 每声间隔时间(毫秒)
 * @param count 响声次数
 * @return true 操作成功
 * @return false 操作失败
 * @details 按指定模式连续响铃多次,适用于报警提示
 */
bool Buzzer::beepPattern(int onMs, int offMs, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (!beep(onMs))
        {
            return false;
        }

        if (i < count - 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(offMs));
        }
    }
    return true;
}

/**
 * @brief 获取蜂鸣器当前状态
 * @return true 蜂鸣器处于打开状态
 * @return false 蜂鸣器处于关闭状态
 */
bool Buzzer::isOn() const
{
    return m_isOn;
}

/**
 * @brief 获取设备打开状态
 * @return true 设备已打开
 * @return false 设备未打开
 */
bool Buzzer::isOpen() const
{
    return m_isOpen;
}
