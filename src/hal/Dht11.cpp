#include "Dht11.h"
#include "../AppConfig.h"

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>

namespace chrono = std::chrono;
using namespace std::chrono_literals;

Dht11& Dht11::getInstance()
{
    static Dht11 instance{APP_DEV_DHT11};
    return instance;
}

Dht11::Dht11(std::string_view path)
    : m_devPath{path}
{
}

Dht11::~Dht11()
{
    closeDevice();
}

bool Dht11::openDevice()
{
    if (m_fd >= 0)
    {
        return true;
    }

    m_fd = ::open(m_devPath.c_str(), O_RDONLY);
    if (m_fd < 0)
    {
        std::cerr << "Failed to open DHT11 device: " << m_devPath << '\n';
        return false;
    }
    return true;
}

void Dht11::closeDevice() noexcept
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool Dht11::updateData()
{
    if (m_fd < 0)
    {
        return false;
    }

    auto now = chrono::steady_clock::now();
    if (now - m_lastReadTime < 1s)
    {
        return true;
    }
    m_lastReadTime = now;

    std::array<char, 32> buf{};
    ssize_t n = ::read(m_fd, buf.data(), buf.size() - 1);

    if (n > 0)
    {
        int temp{};
        int humi{};
        if (std::sscanf(buf.data(), "%d %d", &temp, &humi) == 2)
        {
            m_lastData = Data{.temperature = temp, .humidity = humi};
            return true;
        }
    }
    return false;
}

std::optional<int> Dht11::readTemperature()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData.temperature;
}

std::optional<int> Dht11::readHumidity()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData.humidity;
}

std::optional<Dht11::Data> Dht11::readAll()
{
    if (!updateData())
    {
        return std::nullopt;
    }
    return m_lastData;
}
