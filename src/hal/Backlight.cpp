#include "Backlight.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief 构造函数
 * @param path 背光设备sysfs路径
 * @details 初始化亮度和最大亮度文件路径
 */
Backlight::Backlight(const std::string& path)
    : m_brightnessPath(path + "brightness")
    , m_maxBrightnessPath(path + "/max_brightness")
{
}

/**
 * @brief 析构函数
 */
Backlight::~Backlight()
{
}

/**
 * @brief 设置屏幕亮度
 * @param brightness 亮度值(0-max_brightness)
 * @details 向sysfs brightness文件写入亮度值
 */
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

/**
 * @brief 获取当前屏幕亮度
 * @return int 当前亮度值,失败返回-1
 * @details 从sysfs brightness文件读取亮度值
 */
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
