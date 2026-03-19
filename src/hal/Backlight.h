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

    /// 私有构造函数（单例模式）
    explicit Backlight(std::string_view path);

public:
    /// 析构函数
    ~Backlight() = default;

    // 禁用复制操作
    Backlight(const Backlight&) = delete;
    Backlight& operator=(const Backlight&) = delete;

    // 禁用移动操作
    Backlight(Backlight&&) = delete;
    Backlight& operator=(Backlight&&) = delete;

    /**
     * @brief 获取单例实例
     * @return Backlight& 单例引用
     */
    static Backlight& getInstance();

    /**
     * @brief 设置背光亮度
     * @param level 亮度值（0-max_brightness）
     */
    void setBrightness(int level);

    /**
     * @brief 获取当前背光亮度
     * @return 当前亮度值，失败返回 -1
     */
    [[nodiscard]] int getBrightness() const;
};

#endif // LVGL_APP_BACKLIGHT_H
