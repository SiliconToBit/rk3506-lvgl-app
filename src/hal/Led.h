/**
 * @file Led.h
 * @brief LED硬件控制类
 * @details 通过GPIO控制LED的开关状态
 */

#ifndef LVGL_APP_HAL_LED_H
#define LVGL_APP_HAL_LED_H

#include "FileDescriptor.h"
#include <string>

class Led
{
public:
    explicit Led(std::string_view path);
    ~Led() = default;

    Led(const Led&) = delete;
    Led& operator=(const Led&) = delete;

    Led(Led&&) = delete;
    Led& operator=(Led&&) = delete;

    [[nodiscard]] bool openDevice();

    [[nodiscard]] bool setOn();
    [[nodiscard]] bool setOff();
    [[nodiscard]] bool toggle();

    [[nodiscard]] bool isOn() const;
    [[nodiscard]] bool isOpen() const;

private:
    std::string m_devPath;
    FileDescriptor m_fd;
    bool m_isOn;
    bool m_isOpen;

    bool writeValue(int value);
};

#endif
