/**
 * @file Backlight.h
 * @brief 背光管理类
 * @details 负责背光亮度的设置和获取，使用单例模式
 */

#ifndef LVGL_APP_BACKLIGHT_H
#define LVGL_APP_BACKLIGHT_H

#include <string>

class Backlight
{
private:
    std::string m_brightnessPath;
    std::string m_maxBrightnessPath;

public:
    explicit Backlight(const std::string& path);
    ~Backlight();

    Backlight(const Backlight&) = delete;
    Backlight& operator=(const Backlight&) = delete;

    /**
     * @brief 获取单例实例（使用默认设备路径）
     * @return Backlight& 单例引用
     */
    static Backlight& getInstance();

    void setBrightness(int level);
    int getBrightness() const;
};

#endif // LVGL_APP_BACKLIGHT_H
