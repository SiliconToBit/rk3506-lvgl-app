/**
 * @file Backlight.h
 * @brief 背光管理类
 * @details 负责背光亮度的设置和获取，使用单例模式
 */

#ifndef LVGL_APP_BACKLIGHT_H
#define LVGL_APP_BACKLIGHT_H

#include <string>
#include <string_view>

class Backlight
{
private:
    std::string m_brightnessPath;
    std::string m_maxBrightnessPath;

public:
    explicit Backlight(std::string_view path);
    ~Backlight() = default;

    Backlight(const Backlight&) = delete;
    Backlight& operator=(const Backlight&) = delete;

    static Backlight& getInstance();

    void setBrightness(int level);           // 设置背光亮度
    [[nodiscard]] int getBrightness() const; // 获取背光亮度
};

#endif // LVGL_APP_BACKLIGHT_H
