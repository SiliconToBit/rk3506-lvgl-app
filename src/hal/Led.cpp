#include "Led.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Led::Led(const std::string& gpioPath)
    : m_gpioPath(gpioPath)
    , m_fd(-1)
    , m_isOn(false)
    , m_isOpen(false)
{
}

Led::~Led()
{
    close();
}

bool Led::open()
{
    m_fd = ::open(m_gpioPath.c_str(), O_WRONLY);
    if (m_fd < 0)
    {
        std::cerr << "[Led] Failed to open " << m_gpioPath << std::endl;
        return false;
    }
    m_isOpen = true;
    return true;
}

void Led::close()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
    m_isOpen = false;
}

bool Led::writeValue(int value)
{
    if (m_fd < 0)
    {
        return false;
    }

    const char* val = value ? "1" : "0";
    ssize_t ret = ::write(m_fd, val, 1);
    if (ret < 0)
    {
        std::cerr << "[Led] Write failed" << std::endl;
        return false;
    }

    m_isOn = (value != 0);
    return true;
}

bool Led::setOn()
{
    return writeValue(1);
}

bool Led::setOff()
{
    return writeValue(0);
}

bool Led::toggle()
{
    return m_isOn ? setOff() : setOn();
}

bool Led::isOn() const
{
    return m_isOn;
}

bool Led::isOpen() const
{
    return m_isOpen;
}
