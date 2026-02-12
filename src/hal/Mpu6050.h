#ifndef LVGL_APP_HAL_MPU6050_H
#define LVGL_APP_HAL_MPU6050_H

#include <stdint.h>

class Mpu6050
{
  public:
    Mpu6050();
    ~Mpu6050();

    bool init();   // 打开设备并初始化卡尔曼参数
    bool update(); // 读取一次数据并更新解算结果，返回 true 表示更新成功

    float getRoll() const
    {
        return m_roll;
    }
    float getPitch() const
    {
        return m_pitch;
    }
    float getYaw() const
    {
        return m_yaw;
    }
    float getTemperature() const
    {
        return m_temperature;
    }

  private:
    /* --- 卡尔曼滤波器结构体 --- */
    struct KalmanFilter
    {
        float Q_angle;   // 过程噪声协方差 (角度)
        float Q_bias;    // 过程噪声协方差 (偏差)
        float R_measure; // 测量噪声协方差

        float angle;   // 输出角度
        float bias;    // 输出陀螺仪偏差
        float P[2][2]; // 误差协方差矩阵
    };

    /* 必须与驱动层保持一致 */
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

    int m_fd;
    double m_lastTime;

    KalmanFilter m_kalmanX;
    KalmanFilter m_kalmanY;

    float m_roll;
    float m_pitch;
    float m_yaw;
    float m_temperature;

    float m_gyroBiasX;
    float m_gyroBiasY;
    float m_gyroBiasZ;

    void calibrate();

    void kalmanInit(KalmanFilter* k);
    float kalmanGetAngle(KalmanFilter* k, float newAngle, float newRate, float dt);
    double getTimeSec();
};

#endif // LVGL_APP_HAL_MPU6050_H