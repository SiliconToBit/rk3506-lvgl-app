/**
 * @file Buzzer.h
 * @brief 蜂鸣器控制类
 * @details 通过GPIO或PWM控制蜂鸣器的开关和频率
 */

#ifndef LVGL_APP_HAL_BUZZER_H
#define LVGL_APP_HAL_BUZZER_H

#include <string>

class Buzzer
{
public:
    explicit Buzzer(const std::string& gpioPath);
    ~Buzzer();

    Buzzer(const Buzzer&) = delete;
    Buzzer& operator=(const Buzzer&) = delete;

    bool open();
    void close();

    bool setOn();
    bool setOff();
    bool beep(int durationMs);
    bool beepPattern(int onMs, int offMs, int count);

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
