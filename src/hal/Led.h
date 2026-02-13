/**
 * @file Led.h
 * @brief LED硬件控制类
 * @details 通过GPIO控制LED的开关状态
 */

#ifndef LVGL_APP_HAL_LED_H
#define LVGL_APP_HAL_LED_H

#include <string>

class Led
{
public:
    explicit Led(const std::string& gpioPath);
    ~Led();

    Led(const Led&) = delete;
    Led& operator=(const Led&) = delete;

    bool open();
    void close();

    bool setOn();
    bool setOff();
    bool toggle();

    bool isOn() const;
    bool isOpen() const;

private:
    std::string m_gpioPath;
    int m_fd;
    bool m_isOn;
    bool m_isOpen;

    bool writeValue(int value);
};

#endif
