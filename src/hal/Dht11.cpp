/**
 * @file Dht11.cpp
 * @brief DHT11 温湿度传感器驱动实现
 *
 * 通过读取设备文件获取温湿度数据，支持数据缓存避免频繁读取
 */

#include "Dht11.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>

namespace chrono = std::chrono;
using namespace std::chrono_literals;

/**
 * @brief 构造函数
 * @param path DHT11 设备文件路径，如 "/dev/dht11"
 *
 * 仅保存设备路径，不打开设备（延迟初始化）
 */
Dht11::Dht11(std::string_view path)
    : m_devPath{path}
{
}

/**
 * @brief 打开 DHT11 设备文件
 * @return true 打开成功或设备已打开
 * @return false 打开失败
 *
 * 如果设备已打开则直接返回 true
 * 使用 FileDescriptor 封装，自动管理文件描述符生命周期
 */
bool Dht11::openDevice()
{
    if (m_fd.isValid())
    {
        return true;
    }

    m_fd = FileDescriptor{m_devPath.c_str(), O_RDONLY};
    if (!m_fd.isValid())
    {
        std::cerr << "Failed to open DHT11 device: " << m_devPath << '\n';
        return false;
    }
    return true;
}

/**
 * @brief 更新温湿度数据
 * @return true 更新成功或数据仍在有效期内
 * @return false 更新失败（设备未打开或读取错误）
 *
 * 实现数据缓存机制：
 * - 距离上次读取不足 1 秒时，直接返回缓存数据
 * - 超过 1 秒则重新从设备读取
 *
 * 设备文件格式预期为 "温度 湿度"，如 "25 60"
 */
bool Dht11::updateData()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    // 检查缓存是否有效（1秒内）
    auto now = chrono::steady_clock::now();
    if (now - m_lastReadTime < 1s)
    {
        return true;
    }
    m_lastReadTime = now;

    // 从设备文件读取数据
    std::array<char, 32> buf{};
    ssize_t bytesRead = m_fd.read(buf.data(), buf.size() - 1);

    if (bytesRead > 0)
    {
        int temp{};
        int humi{};
        // 解析 "温度 湿度" 格式
        if (std::sscanf(buf.data(), "%d %d", &temp, &humi) == 2)
        {
            m_lastData = Data{.temperature = temp, .humidity = humi};
            return true;
        }
    }
    return false;
}

/**
 * @brief 读取温度值
 * @return std::optional<int> 温度值（摄氏度），失败返回 nullopt
 *
 * 内部调用 updateData()，自动处理缓存和更新
 */
std::optional<int> Dht11::readTemperature()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData.temperature;
}

/**
 * @brief 读取湿度值
 * @return std::optional<int> 湿度值（百分比），失败返回 nullopt
 *
 * 内部调用 updateData()，自动处理缓存和更新
 */
std::optional<int> Dht11::readHumidity()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData.humidity;
}

/**
 * @brief 读取温湿度数据
 * @return std::optional<Data> 包含温度和湿度的结构体，失败返回 nullopt
 *
 * 内部调用 updateData()，自动处理缓存和更新
 */
std::optional<Dht11::Data> Dht11::readAll()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData;
}
