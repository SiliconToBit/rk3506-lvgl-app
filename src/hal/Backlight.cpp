#include "Backlight.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Backlight::Backlight(const std::string& path)
    : m_brightnessPath(path + "brightness")
    , m_maxBrightnessPath(path + "/max_brightness")
{
}

Backlight::~Backlight()
{
}

void Backlight::setBrightness(int brightness)
{
    FILE* file = fopen(m_brightnessPath.c_str(), "w");
    if (!file)
    {
        std::cerr << "Failed to open file " << m_brightnessPath.c_str() << std::endl;
        return;
    }

    fprintf(file, "%d\n", brightness);
    fclose(file);
}

int Backlight::getBrightness() const
{
    FILE* file = fopen(m_brightnessPath.c_str(), "r");
    if (!file)
    {
        std::cerr << "Failed to open file " << m_brightnessPath.c_str() << std::endl;
        return -1;
    }
    int value;
    fscanf(file, "%d", &value);
    fclose(file);
    return value;
}
