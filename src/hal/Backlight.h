//
// Created by gm on 2025/12/3.
//

#ifndef LVGL_DEMO_BACKLIGHT_H
#define LVGL_DEMO_BACKLIGHT_H
#include <pstl/algorithm_impl.h>

class Backlight
{
private:
    std::string brightness_path;
    std::string max_brightness_path;
public:
    Backlight(std::string path);
    ~Backlight();
    void setBrightness(int level);
    int getBrightness() const;
};


#endif //LVGL_DEMO_BACKLIGHT_H
