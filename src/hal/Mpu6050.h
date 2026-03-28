/**
 * @file Mpu6050.h
 * @brief MPU6050 六轴传感器驱动
 *
 * 负责读取 MPU6050 加速度计和陀螺仪数据，
 * 使用卡尔曼滤波融合计算姿态角（横滚角、俯仰角）
 */

#pragma once

#include "FileDescriptor.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

/**
 * @class Mpu6050
 * @brief MPU6050 传感器管理类
 *
 * 使用延迟初始化模式，需要调用 openDevice() 打开设备后才能读取数据
 */
class Mpu6050
{
public:
    /**
     * @brief 传感器数据结构
     */
    struct Data
    {
        float roll;        ///< 横滚角（度）
        float pitch;       ///< 俯仰角（度）
        float yaw;         ///< 偏航角（度）
        float temperature; ///< 温度（摄氏度）
    };

    /**
     * @brief 获取单例实例
     * @return Mpu6050& 单例引用
     */
    [[nodiscard]] static Mpu6050& getInstance();

    /**
     * @brief 构造函数
     * @param path 设备文件路径
     */
    explicit Mpu6050(std::string_view path);

    /// 析构函数（由 FileDescriptor 自动管理资源）
    ~Mpu6050() = default;

    // 禁用复制操作
    Mpu6050(const Mpu6050&) = delete;
    Mpu6050& operator=(const Mpu6050&) = delete;

    // 禁用移动操作（单例模式）
    Mpu6050(Mpu6050&&) = delete;
    Mpu6050& operator=(Mpu6050&&) = delete;

    /**
     * @brief 打开设备
     * @return true 打开成功
     * @return false 打开失败
     */
    [[nodiscard]] bool openDevice();

    /**
     * @brief 更新传感器数据
     * @return true 更新成功
     * @return false 更新失败（设备未打开或读取错误）
     */
    [[nodiscard]] bool update();

    /**
     * @brief 获取横滚角
     * @return 横滚角（度）
     */
    [[nodiscard]] float getRoll() const noexcept
    {
        return m_roll;
    }

    /**
     * @brief 获取俯仰角
     * @return 俯仰角（度）
     */
    [[nodiscard]] float getPitch() const noexcept
    {
        return m_pitch;
    }

    /**
     * @brief 获取偏航角
     * @return 偏航角（度）
     */
    [[nodiscard]] float getYaw() const noexcept
    {
        return m_yaw;
    }

    /**
     * @brief 获取温度
     * @return 温度（摄氏度）
     */
    [[nodiscard]] float getTemperature() const noexcept
    {
        return m_temperature;
    }

    /**
     * @brief 获取所有传感器数据
     * @return 包含姿态角和温度的数据结构
     */
    [[nodiscard]] Data getData() const noexcept
    {
        return Data{m_roll, m_pitch, m_yaw, m_temperature};
    }

    /**
     * @brief 检查设备是否已打开
     * @return true 设备已打开
     * @return false 设备未打开
     */
    [[nodiscard]] bool isOpen() const noexcept
    {
        return m_fd.isValid();
    }

private:
    /// 原始数据结构（与内核驱动格式对应）
    struct RawData
    {
        uint8_t accel_x_h;
        uint8_t accel_x_l;
        uint8_t accel_y_h;
        uint8_t accel_y_l;
        uint8_t accel_z_h;
        uint8_t accel_z_l;
        uint8_t temp_h;
        uint8_t temp_l;
        uint8_t gyro_x_h;
        uint8_t gyro_x_l;
        uint8_t gyro_y_h;
        uint8_t gyro_y_l;
        uint8_t gyro_z_h;
        uint8_t gyro_z_l;
    };

    /// 卡尔曼滤波器状态
    struct KalmanState
    {
        float Q_angle{0.001F};  ///< 角度过程噪声
        float Q_bias{0.003F};   ///< 零偏过程噪声
        float R_measure{0.03F}; ///< 测量噪声
        float angle{0.0F};      ///< 估计角度
        float bias{0.0F};       ///< 估计零偏

        std::array<std::array<float, 2>, 2> P{{{{0.0F, 0.0F}}, {{0.0F, 0.0F}}}}; ///< 误差协方差矩阵
    };


    
    std::string m_devPath;                            ///< 设备路径
    FileDescriptor m_fd;                              ///< 文件描述符
    KalmanState m_kalmanX;                            ///< X轴卡尔曼滤波器
    KalmanState m_kalmanY;                            ///< Y轴卡尔曼滤波器
    float m_roll{0.0F};                               ///< 横滚角
    float m_pitch{0.0F};                              ///< 俯仰角
    float m_yaw{0.0F};                                ///< 偏航角
    float m_temperature{0.0F};                        ///< 温度
    float m_gyroBiasX{0.0F};                          ///< X轴陀螺仪零偏
    float m_gyroBiasY{0.0F};                          ///< Y轴陀螺仪零偏
    float m_gyroBiasZ{0.0F};                          ///< Z轴陀螺仪零偏
    std::chrono::steady_clock::time_point m_lastTime; ///< 上次更新时间

    /// 陀螺仪校准
    void calibrate();

    /// 初始化卡尔曼滤波器
    void kalmanInit(KalmanState& k);

    /// 卡尔曼滤波更新
    [[nodiscard]] float kalmanUpdate(KalmanState& k, float newAngle, float newRate, float dt);
};