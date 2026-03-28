#include "Buzzer.h"
#include <fcntl.h>
#include <string_view>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief 构造函数
 * @param gpioPath GPIO设备路径,如 "/sys/class/gpio/gpio12/value"
 * @details 初始化蜂鸣器对象,设置初始状态为关闭未打开
 */
Buzzer::Buzzer(std::string_view path)
    : m_devPath{path}
    , m_isOn{false}
    , m_isOpen{false}
{
}

/**
 * @brief 打开GPIO设备
 * @return true 打开成功
 * @return false 打开失败
 * @details 以只写方式打开GPIO设备文件,用于控制蜂鸣器
 */
bool Buzzer::openDevice()
{
    if (m_fd.isValid())
    {
        return true;
    }
    m_fd = FileDescriptor(m_devPath.c_str(), O_WRONLY);
    if (!m_fd.isValid())
    {
        std::cerr << "[Buzzer] Failed to open " << m_devPath << '\n';
        return false;
    }
    m_isOpen = true;
    return true;
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
    if (!m_fd.isValid())
    {
        return false;
    }

    const char* val = static_cast<bool>(value) ? "1" : "0";
    ssize_t ret = m_fd.write(val, 1);
    if (ret != 1)
    {
        std::cerr << "[Buzzer] Write failed" << '\n';
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
