#include "Buzzer.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

Buzzer::Buzzer(const std::string& gpioPath)
    : m_gpioPath(gpioPath)
    , m_fd(-1)
    , m_isOn(false)
    , m_isOpen(false)
{
}

Buzzer::~Buzzer()
{
    close();
}

bool Buzzer::open()
{
    m_fd = ::open(m_gpioPath.c_str(), O_WRONLY);
    if (m_fd < 0)
    {
        std::cerr << "[Buzzer] Failed to open " << m_gpioPath << std::endl;
        return false;
    }
    m_isOpen = true;
    return true;
}

void Buzzer::close()
{
    if (m_fd >= 0)
    {
        setOff();
        ::close(m_fd);
        m_fd = -1;
    }
    m_isOpen = false;
}

bool Buzzer::writeValue(int value)
{
    if (m_fd < 0)
    {
        return false;
    }

    const char* val = value ? "1" : "0";
    ssize_t ret = ::write(m_fd, val, 1);
    if (ret < 0)
    {
        std::cerr << "[Buzzer] Write failed" << std::endl;
        return false;
    }

    m_isOn = (value != 0);
    return true;
}

bool Buzzer::setOn()
{
    return writeValue(1);
}

bool Buzzer::setOff()
{
    return writeValue(0);
}

bool Buzzer::beep(int durationMs)
{
    if (!setOn())
    {
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));

    return setOff();
}

bool Buzzer::beepPattern(int onMs, int offMs, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (!beep(onMs))
        {
            return false;
        }

        if (i < count - 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(offMs));
        }
    }
    return true;
}

bool Buzzer::isOn() const
{
    return m_isOn;
}

bool Buzzer::isOpen() const
{
    return m_isOpen;
}
