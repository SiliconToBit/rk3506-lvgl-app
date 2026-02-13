#include "Led.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

/**
 * @brief 构造函数
 * @param gpioPath GPIO设备路径,如 "/sys/class/gpio/gpio10/value"
 * @details 初始化LED对象,设置初始状态为关闭未打开
 */
Led::Led(const std::string& gpioPath)
    : m_gpioPath(gpioPath)
    , m_fd(-1)
    , m_isOn(false)
    , m_isOpen(false)
{
}

/**
 * @brief 析构函数
 * @details 自动关闭GPIO设备,释放资源
 */
Led::~Led()
{
    close();
}

/**
 * @brief 打开GPIO设备
 * @return true 打开成功
 * @return false 打开失败
 * @details 以只写方式打开GPIO设备文件,用于控制LED开关
 */
bool Led::open()
{
    m_fd = ::open(m_gpioPath.c_str(), O_WRONLY);
    if (m_fd < 0)
    {
        std::cerr << "[Led] Failed to open " << m_gpioPath << std::endl;
        return false;
    }
    m_isOpen = true;
    return true;
}

/**
 * @brief 关闭GPIO设备
 * @details 关闭文件描述符,重置状态标志
 */
void Led::close()
{
    if (m_fd >= 0)
    {
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
 * @details 将0或1写入GPIO设备,控制LED开关状态
 */
bool Led::writeValue(int value)
{
    if (m_fd < 0)
    {
        return false;
    }

    const char* val = value ? "1" : "0";
    ssize_t ret = ::write(m_fd, val, 1);
    if (ret < 0)
    {
        std::cerr << "[Led] Write failed" << std::endl;
        return false;
    }

    m_isOn = (value != 0);
    return true;
}

/**
 * @brief 打开LED
 * @return true 操作成功
 * @return false 操作失败
 */
bool Led::setOn()
{
    return writeValue(1);
}

/**
 * @brief 关闭LED
 * @return true 操作成功
 * @return false 操作失败
 */
bool Led::setOff()
{
    return writeValue(0);
}

/**
 * @brief 切换LED状态
 * @return true 操作成功
 * @return false 操作失败
 * @details 根据当前状态切换LED开关
 */
bool Led::toggle()
{
    return m_isOn ? setOff() : setOn();
}

/**
 * @brief 获取LED当前状态
 * @return true LED处于打开状态
 * @return false LED处于关闭状态
 */
bool Led::isOn() const
{
    return m_isOn;
}

/**
 * @brief 获取设备打开状态
 * @return true 设备已打开
 * @return false 设备未打开
 */
bool Led::isOpen() const
{
    return m_isOpen;
}
