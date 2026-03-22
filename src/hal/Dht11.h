#pragma once

#include "FileDescriptor.h"

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

class Dht11
{
public:
    struct Data
    {
        int temperature{0}; // 温度,单位:摄氏度
        int humidity{0};    // 湿度,单位:百分比
    };

    explicit Dht11(std::string_view path); // 构造函数

    Dht11(const Dht11&) = delete;            // 禁用复制构造函数
    Dht11& operator=(const Dht11&) = delete; // 禁用复制赋值运算符
    Dht11(Dht11&&) = delete;                 // 禁用移动构造函数
    Dht11& operator=(Dht11&&) = delete;      // 禁用移动赋值运算符

    [[nodiscard]] bool openDevice(); // 打开设备

    [[nodiscard]] std::optional<int> readTemperature(); // 读取温度
    [[nodiscard]] std::optional<int> readHumidity();    // 读取湿度
    [[nodiscard]] std::optional<Data> readAll();        // 读取温度和湿度,并返回结构体

    [[nodiscard]] bool isOpen() const noexcept // 是否打开设备
    {
        return m_fd.isValid();
    }

private:
    std::string m_devPath{};                                // 设备路径,如 "/sys/class/gpio/gpio12/value"
    FileDescriptor m_fd{};                                  // 设备文件描述符
    Data m_lastData{};                                      // 最后一次取到的温湿度数据
    std::chrono::steady_clock::time_point m_lastReadTime{}; // 最后一次读取时间

    [[nodiscard]] bool updateData(); // 更新缓存和数据
};
