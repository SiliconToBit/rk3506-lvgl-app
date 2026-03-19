#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

class Dht11
{
public:
    struct Data
    {
        int temperature;
        int humidity;
    };

private:
    std::string m_devPath;                                  // 设备路径
    int m_fd{-1};                                           // 设备文件描述符
    Data m_lastData{};                                      // 最近一次读取到的数据
    std::chrono::steady_clock::time_point m_lastReadTime{}; // 最近一次读取时间

    [[nodiscard]] bool updateData(); // 更新数据

public:
    explicit Dht11(std::string_view path); // 构造函数
    ~Dht11();                              // 析构函数

    Dht11(const Dht11&) = delete;            // 禁用复制构造函数
    Dht11& operator=(const Dht11&) = delete; // 禁用复制赋值运算符
    Dht11(Dht11&&) = delete;                 // 禁用移动构造函数
    Dht11& operator=(Dht11&&) = delete;      // 禁用移动赋值运算符

    [[nodiscard]] static Dht11& getInstance(); // 获取单例实例

    [[nodiscard]] bool openDevice(); // 打开设备
    void closeDevice() noexcept;     // 关闭设备

    [[nodiscard]] std::optional<int> readTemperature(); // 读取温度
    [[nodiscard]] std::optional<int> readHumidity();    // 读取湿度
    [[nodiscard]] std::optional<Data> readAll();        // 读取所有数据

    [[nodiscard]] bool isOpen() const noexcept // 检查设备是否打开
    {
        return m_fd >= 0;
    }
};
