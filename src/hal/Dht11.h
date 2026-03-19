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

private:
    std::string m_devPath;                                  // 设备路径,如 "/dev/dht11"
    FileDescriptor m_fd;                                    // 设备文件描述符
    Data m_lastData{};                                      // 上次读取的数据
    std::chrono::steady_clock::time_point m_lastReadTime{}; // 上次读取的时间点

    [[nodiscard]] bool updateData(); // 更新数据

public:
    explicit Dht11(std::string_view path); // 构造函数

    Dht11(const Dht11&) = delete;            // 禁用复制构造函数
    Dht11& operator=(const Dht11&) = delete; // 禁用复制赋值运算符
    Dht11(Dht11&&) = delete;                 // 禁止移动构造函数
    Dht11& operator=(Dht11&&) = delete;      // 禁止移动赋值运算符

    [[nodiscard]] bool openDevice(); // 打开设备

    [[nodiscard]] std::optional<int> readTemperature(); // 读取温度
    [[nodiscard]] std::optional<int> readHumidity();    // 读取湿度
    [[nodiscard]] std::optional<Data> readAll();        // 读取所有数据

    [[nodiscard]] bool isOpen() const noexcept // 是否打开设备
    {
        return m_fd.isValid();
    }
};
