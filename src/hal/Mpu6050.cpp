/**
 * @file Mpu6050.cpp
 * @brief MPU6050 六轴传感器驱动实现
 *
 * 通过 I2C 设备文件读取加速度计和陀螺仪原始数据，
 * 使用卡尔曼滤波融合计算稳定的姿态角
 */

#include "Mpu6050.h"
#include "../AppConfig.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace chrono = std::chrono;

/// 弧度转角度常量
constexpr float RAD_TO_DEG = 57.295779513082320876F;

/// 加速度计灵敏度（±2g 量程）
constexpr float ACCEL_SCALE = 16384.0F;

/// 陀螺仪灵敏度（±250°/s 量程）
constexpr float GYRO_SCALE = 131.0F;

/// 温度传感器偏移
constexpr float TEMP_OFFSET = 36.53F;

/// 温度传感器比例
constexpr float TEMP_SCALE = 340.0F;

/// 校准采样次数
constexpr int CALIB_COUNT = 500;

/// 陀螺仪死区阈值
constexpr float GYRO_DEADZONE = 0.05F;

/**
 * @brief 获取单例实例
 * @return Mpu6050& 单例引用
 *
 * 使用静态局部变量实现线程安全的单例模式（Meyer's Singleton）
 */
Mpu6050& Mpu6050::getInstance()
{
    static Mpu6050 instance{APP_DEV_MPU6050};
    return instance;
}

/**
 * @brief 构造函数
 * @param path MPU6050 设备文件路径
 *
 * 仅保存设备路径，不打开设备（延迟初始化）
 */
Mpu6050::Mpu6050(std::string_view path)
    : m_devPath{path}
{
    kalmanInit(m_kalmanX);
    kalmanInit(m_kalmanY);
}

/**
 * @brief 打开 MPU6050 设备
 * @return true 打开成功
 * @return false 打开失败
 *
 * 打开设备文件并执行陀螺仪校准
 * 使用 FileDescriptor 封装，自动管理文件描述符生命周期
 */
bool Mpu6050::openDevice()
{
    if (m_fd.isValid())
    {
        return true;
    }

    m_fd = FileDescriptor{m_devPath.c_str(), O_RDONLY};
    if (!m_fd.isValid())
    {
        std::cerr << "Failed to open MPU6050 device: " << m_devPath << '\n';
        return false;
    }

    calibrate();
    m_lastTime = chrono::steady_clock::now();
    return true;
}

/**
 * @brief 陀螺仪校准
 *
 * 采集多个样本计算陀螺仪零点偏移量
 * 校准期间应保持传感器静止
 */
void Mpu6050::calibrate()
{
    std::cout << "Keep sensor still! Calibrating gyro...\n";

    double gx_sum = 0.0;
    double gy_sum = 0.0;
    double gz_sum = 0.0;
    RawData raw{};

    for (int i = 0; i < CALIB_COUNT; ++i)
    {
        if (m_fd.read(&raw, sizeof(raw)) == sizeof(raw))
        {
            auto gx = static_cast<int16_t>((raw.gyro_x_h << 8) | raw.gyro_x_l);
            auto gy = static_cast<int16_t>((raw.gyro_y_h << 8) | raw.gyro_y_l);
            auto gz = static_cast<int16_t>((raw.gyro_z_h << 8) | raw.gyro_z_l);

            gx_sum += gx;
            gy_sum += gy;
            gz_sum += gz;

            usleep(2000);
        }
    }

    m_gyroBiasX = static_cast<float>(gx_sum / CALIB_COUNT / GYRO_SCALE);
    m_gyroBiasY = static_cast<float>(gy_sum / CALIB_COUNT / GYRO_SCALE);
    m_gyroBiasZ = static_cast<float>(gz_sum / CALIB_COUNT / GYRO_SCALE);

    std::cout << "Calibration Done! Bias X:" << m_gyroBiasX << " Y:" << m_gyroBiasY << " Z:" << m_gyroBiasZ << '\n';
}

/**
 * @brief 初始化卡尔曼滤波器
 * @param k 卡尔曼滤波器状态
 */
void Mpu6050::kalmanInit(KalmanState& k)
{
    k.Q_angle = 0.001F;
    k.Q_bias = 0.003F;
    k.R_measure = 0.03F;
    k.angle = 0.0F;
    k.bias = 0.0F;
    k.P[0][0] = 0.0F;
    k.P[0][1] = 0.0F;
    k.P[1][0] = 0.0F;
    k.P[1][1] = 0.0F;
}

/**
 * @brief 卡尔曼滤波更新
 * @param k 卡尔曼滤波器状态
 * @param newAngle 加速度计测量的角度（观测值）
 * @param newRate 陀螺仪测量的角速度
 * @param dt 时间间隔（秒）
 * @return 滤波后的角度
 *
 * 融合加速度计和陀螺仪数据，输出稳定的角度估计
 */
float Mpu6050::kalmanUpdate(KalmanState& k, float newAngle, float newRate, float dt)

{
    // 预测步骤
    float rate = newRate - k.bias;
    k.angle += dt * rate;

    // 更新误差协方差
    k.P[0][0] += dt * (dt * k.P[1][1] - k.P[0][1] - k.P[1][0] + k.Q_angle);
    k.P[0][1] -= dt * k.P[1][1];
    k.P[1][0] -= dt * k.P[1][1];
    k.P[1][1] += k.Q_bias * dt;

    // 计算卡尔曼增益
    float S = k.P[0][0] + k.R_measure;
    float K[2] = {k.P[0][0] / S, k.P[1][0] / S};

    // 更新步骤
    float y = newAngle - k.angle;
    k.angle += K[0] * y;
    k.bias += K[1] * y;

    // 更新误差协方差
    float P00_temp = k.P[0][0];
    float P01_temp = k.P[0][1];
    k.P[0][0] -= K[0] * P00_temp;
    k.P[0][1] -= K[0] * P01_temp;
    k.P[1][0] -= K[1] * P00_temp;
    k.P[1][1] -= K[1] * P01_temp;

    return k.angle;
}

/**
 * @brief 更新传感器数据
 * @return true 更新成功
 * @return false 更新失败（设备未打开或读取错误）
 *
 * 读取原始数据，转换为物理单位，应用卡尔曼滤波计算姿态角
 */
bool Mpu6050::update()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    RawData raw{};
    if (m_fd.read(&raw, sizeof(raw)) != sizeof(raw))
    {
        return false;
    }

    // 计算时间间隔
    auto now = chrono::steady_clock::now();
    auto duration = now - m_lastTime;
    m_lastTime = now;
    float dt = chrono::duration<float>(duration).count();
    if (dt <= 0.0F)
    {
        dt = 0.01F;
    }

    // 解析原始数据（大端序）
    auto ax_raw = static_cast<int16_t>((raw.accel_x_h << 8) | raw.accel_x_l);
    auto ay_raw = static_cast<int16_t>((raw.accel_y_h << 8) | raw.accel_y_l);
    auto az_raw = static_cast<int16_t>((raw.accel_z_h << 8) | raw.accel_z_l);
    auto gx_raw = static_cast<int16_t>((raw.gyro_x_h << 8) | raw.gyro_x_l);
    auto gy_raw = static_cast<int16_t>((raw.gyro_y_h << 8) | raw.gyro_y_l);
    auto gz_raw = static_cast<int16_t>((raw.gyro_z_h << 8) | raw.gyro_z_l);
    auto temp_raw = static_cast<int16_t>((raw.temp_h << 8) | raw.temp_l);

    // 转换为物理单位
    float ax = ax_raw / ACCEL_SCALE;
    float ay = ay_raw / ACCEL_SCALE;
    float az = az_raw / ACCEL_SCALE;
    float gx = (gx_raw / GYRO_SCALE) - m_gyroBiasX;
    float gy = (gy_raw / GYRO_SCALE) - m_gyroBiasY;
    float gz = (gz_raw / GYRO_SCALE) - m_gyroBiasZ;

    // 计算温度
    m_temperature = temp_raw / TEMP_SCALE + TEMP_OFFSET;

    // 陀螺仪死区滤波
    if (std::fabs(gx) < GYRO_DEADZONE)
        gx = 0.0F;
    if (std::fabs(gy) < GYRO_DEADZONE)
        gy = 0.0F;
    if (std::fabs(gz) < GYRO_DEADZONE)
        gz = 0.0F;

    // 从加速度计计算角度
    float acc_roll = std::atan2(ay, az) * RAD_TO_DEG;
    float acc_pitch = std::atan2(-ax, std::sqrt((ay * ay) + (az * az))) * RAD_TO_DEG;

    // 卡尔曼滤波融合
    m_roll = kalmanUpdate(m_kalmanX, acc_roll, gx, dt);
    m_pitch = kalmanUpdate(m_kalmanY, acc_pitch, gy, dt);

    return true;
}
