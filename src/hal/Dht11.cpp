#include "Dht11.h"
#include "../AppConfig.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

/**
 * @brief 获取单例实例
 * @return Dht11& 单例引用
 * @details 使用默认设备路径 APP_DEV_DHT11
 */
Dht11& Dht11::getInstance()
{
    static Dht11 instance(APP_DEV_DHT11);
    return instance;
}

/**
 * @brief 构造函数
 * @param path 设备文件路径,如 "/dev/dht11"
 * @details 初始化DHT11对象,设置文件描述符为-1(未打开),温度和湿度初始值为0
 */
Dht11::Dht11(const std::string& path) : m_devPath(path), m_fd(-1), m_lastTemp(0), m_lastHumi(0), m_lastReadTime(0) {}

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
        std::cerr << "Failed to open DHT11 device: " << m_devPath << '\n';
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
 * @brief 更新温湿度数据
 * @details 从设备文件读取原始数据并解析为温度和湿度值
 *          数据格式: "temperature humidity" (如 "25 60")
 *          读取失败时保持上一次的有效数据
 */
void Dht11::updateData()
{
    if (m_fd < 0)
        return;

    // 限制读取频率(每秒最多1次)
    time_t now = time(nullptr);
    if (now - m_lastReadTime < 1)
        return;
    m_lastReadTime = now;

    char buf[32] = {0};
    ssize_t n = read(m_fd, buf, sizeof(buf) - 1);
    if (n > 0)
    {
        buf[n] = '\0';
        int temp = 0, humi = 0;
        if (sscanf(buf, "%d %d", &temp, &humi) == 2)
        {
            m_lastTemp = temp;
            m_lastHumi = humi;
        }
    }
}

/**
 * @brief 读取温度值
 * @return int 温度值(摄氏度),失败返回0
 * @details 先更新数据,然后返回缓存的温度值
 */
int Dht11::readTemperature()
{
    updateData();
    return m_lastTemp;
}

/**
 * @brief 读取湿度值
 * @return int 湿度值(%),失败返回0
 * @details 先更新数据,然后返回缓存的湿度值
 */
int Dht11::readHumidity()
{
    updateData();
    return m_lastHumi;
}
