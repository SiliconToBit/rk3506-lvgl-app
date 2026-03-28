/**
 * @file Buzzer.h
 * @brief 蜂鸣器控制类
 * @details 通过GPIO或PWM控制蜂鸣器的开关和频率
 */

#pragma once

#include "FileDescriptor.h"
#include <optional>
#include <string>
#include <string_view>

class Buzzer
{
public:
    explicit Buzzer(std::string_view path); // 构造函数
    ~Buzzer() = default;                    // 析构函数

    Buzzer(const Buzzer&) = delete;            // 禁用复制构造函数
    Buzzer& operator=(const Buzzer&) = delete; // 禁用复制赋值运算符
    Buzzer(Buzzer&&) = delete;                 // 禁止移动构造函数
    Buzzer& operator=(Buzzer&&) = delete;      // 禁止移动赋值运算符

    [[nodiscard]] bool openDevice(); // 打开设备

    [[nodiscard]] bool setOn();                                     // 设置蜂鸣器为开状态
    [[nodiscard]] bool setOff();                                    // 设置蜂鸣器为关状态
    [[nodiscard]] bool beep(int durationMs);                        // 指续蜂鸣
    [[nodiscard]] bool beepPattern(int onMs, int offMs, int count); // 指续蜂鸣,并重复指定次数

    [[nodiscard]] bool isOn() const noexcept // 是否蜂鸣器为开状态
    {
        return m_isOn;
    }
    [[nodiscard]] bool isOpen() const noexcept // 是否打开设备
    {
        return m_fd.isValid();
    }

private:
    std::string m_devPath{}; // 设备路径,如 "/sys/class/gpio/gpio12/value"
    FileDescriptor m_fd{};   // 设备文件描述符
    bool m_isOn{};           // 是否蜂鸣器为开状态
    bool m_isOpen{};         // 是否打开设备

    bool writeValue(int value); // 写入值到设备文件
};
