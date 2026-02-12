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
