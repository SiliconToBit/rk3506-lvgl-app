#include "Dht11.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

Dht11::Dht11(std::string path) : devPath(path), fd(-1), lastTemp(0), lastHumi(0), lastReadTime(0)
{
}

Dht11::~Dht11()
{
    closeDevice();
}

bool Dht11::openDevice()
{
    if (fd >= 0)
        return true;
    fd = open(devPath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        std::cerr << "Failed to open DHT11 device: " << devPath << std::endl;
        return false;
    }
    return true;
}

void Dht11::closeDevice()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}

void Dht11::updateData()
{
    if (fd < 0)
        return;

    time_t now = time(NULL);
    // 只有当距离上次读取超过1秒时才重新读取，避免频繁读取导致驱动失败或数据错误
    if (now - lastReadTime >= 1)
    {
        unsigned char data[2];

        // 尝试读取传感器数据: [0]=湿度, [1]=温度
        if (read(fd, data, 2) == 2)
        {
            lastHumi = data[0];
            lastTemp = data[1];
            lastReadTime = now;
        }
        // 如果读取失败，保持旧值或者可以考虑清零，这里选择保持旧值以便UI不闪烁
    }
}

int Dht11::readTemperature()
{
    if (fd < 0)
        return 0;

    updateData(); // 尝试更新数据
    return lastTemp;
}

int Dht11::readHumidity()
{
    if (fd < 0)
        return 0;

    updateData(); // 尝试更新数据
    return lastHumi;
}
