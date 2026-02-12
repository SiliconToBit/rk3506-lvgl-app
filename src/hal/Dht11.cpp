#include "Dht11.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

Dht11::Dht11(const std::string& path)
    : m_devPath(path)
    , m_fd(-1)
    , m_lastTemp(0)
    , m_lastHumi(0)
    , m_lastReadTime(0)
{
}

Dht11::~Dht11()
{
    close();
}

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

void Dht11::close()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

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

int Dht11::readTemperature()
{
    if (m_fd < 0)
        return 0;
    updateData();
    return m_lastTemp;
}

int Dht11::readHumidity()
{
    if (m_fd < 0)
        return 0;
    updateData();
    return m_lastHumi;
}
