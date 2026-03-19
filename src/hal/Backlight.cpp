#include "Backlight.h"
#include "../AppConfig.h"

#include <iostream>
#include <unistd.h>

/**
 * @brief 获取单例实例
 * @return Backlight& 单例引用
 * @details 使用默认设备路径 APP_DEV_BACKLIGHT
 */
Backlight& Backlight::getInstance()
{
    static Backlight instance(APP_DEV_BACKLIGHT);
    return instance;
}

/**
 * @brief 构造函数
 * @param path 背光设备sysfs路径
 * @details 初始化亮度和最大亮度文件路径
 */
Backlight::Backlight(std::string_view path)
    : m_brightnessPath{std::string(path) + "/brightness"}
    , m_maxBrightnessPath{std::string(path) + "/max_brightness"}
{
}

/**
 * @brief 设置屏幕亮度
 * @param level 亮度值(0-max_brightness)
 * @details 向sysfs brightness文件写入亮度值
 */
void Backlight::setBrightness(int level)
{
    FILE* file = fopen(m_brightnessPath.c_str(), "w");
    if (file == nullptr)
    {
        std::cerr << "Failed to open file " << m_brightnessPath.c_str() << '\n';
        return;
    }

    fprintf(file, "%d\n", level);
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
    if (file == nullptr)
    {
        std::cerr << "Failed to open file " << m_brightnessPath.c_str() << '\n';
        return -1;
    }
    int value;
    fscanf(file, "%d", &value);
    fclose(file);
    return value;
}
