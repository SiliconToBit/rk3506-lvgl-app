/**
 * @file Backlight.h
 * @brief 背光管理类
 * @details 负责背光亮度的设置和获取
 */

#ifndef LVGL_APP_HAL_BACKLIGHT_H
#define LVGL_APP_HAL_BACKLIGHT_H

#include <string>

class Backlight
{
private:
    std::string m_brightnessPath;
    std::string m_maxBrightnessPath;

public:
    explicit Backlight(const std::string& path);
    ~Backlight();

    void setBrightness(int level);
    int getBrightness() const;
};

#endif // LVGL_APP_HAL_BACKLIGHT_H
