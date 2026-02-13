#include "Dht11.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

/**
 * @brief 构造函数
 * @param path 设备文件路径,如 "/dev/dht11"
 * @details 初始化DHT11对象,设置文件描述符为-1(未打开),温度和湿度初始值为0
 */
Dht11::Dht11(const std::string& path)
    : m_devPath(path)
    , m_fd(-1)
    , m_lastTemp(0)
    , m_lastHumi(0)
    , m_lastReadTime(0)
{
}

/**
 * @brief 析构函数
 * @details 自动关闭设备文件,释放资源
 */
Dht11::~Dht11()
{
    close();
}

/**
 * @brief 打开设备文件
 * @return true 打开成功
 * @return false 打开失败
 * @details 以只读方式打开DHT11设备文件
 */
bool Dht11::open()
{
    if (m_fd >= 0)
        return true;
    m_fd = ::open(m_devPath.c_str(), O_RDONLY);
    if (m_fd < 0)
    {
        std::cerr << "Failed to open DHT11 device: " << m_devPath << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief 关闭设备文件
 * @details 关闭文件描述符,重置为-1
 */
void Dht11::close()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

/**
 * @brief 从设备读取数据
 * @details 从设备读取2字节数据(湿度+温度),每秒最多读取一次
 */
void Dht11::updateData()
{
    if (m_fd < 0)
        return;

    time_t now = time(NULL);
    if (now - m_lastReadTime >= 1)
    {
        unsigned char data[2];
        if (read(m_fd, data, 2) == 2)
        {
            m_lastHumi = data[0];
            m_lastTemp = data[1];
            m_lastReadTime = now;
        }
    }
}

/**
 * @brief 读取温度
 * @return int 温度值(摄氏度)
 * @return 0 设备未打开或读取失败
 * @details 先调用updateData()更新数据,然后返回温度值
 */
int Dht11::readTemperature()
{
    if (m_fd < 0)
        return 0;
    updateData();
    return m_lastTemp;
}

/**
 * @brief 读取湿度
 * @return int 湿度值(%)
 * @return 0 设备未打开或读取失败
 * @details 先调用updateData()更新数据,然后返回湿度值
 */
int Dht11::readHumidity()
{
    if (m_fd < 0)
        return 0;
    updateData();
    return m_lastHumi;
}
