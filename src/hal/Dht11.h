/**
 * @file Dht11.h
 * @brief DHT11 温湿度传感器管理类
 * @details 负责读取 DHT11 传感器的温度和湿度数据，使用单例模式
 */

#ifndef LVGL_APP_HAL_DHT11_H
#define LVGL_APP_HAL_DHT11_H

#include <ctime>
#include <string>

class Dht11
{
private:
    std::string m_devPath;
    int m_fd;
    int m_lastTemp;
    int m_lastHumi;
    time_t m_lastReadTime;

    void updateData();

public:
    explicit Dht11(const std::string& path);
    ~Dht11();

    Dht11(const Dht11&) = delete;
    Dht11& operator=(const Dht11&) = delete;

    /**
     * @brief 获取单例实例（使用默认设备路径）
     * @return Dht11& 单例引用
     */
    static Dht11& getInstance();

    bool open();
    void close();
    int readTemperature();
    int readHumidity();
};

#endif // LVGL_APP_HAL_DHT11_H
