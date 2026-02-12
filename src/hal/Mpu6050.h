#ifndef LVGL_APP_HAL_MPU6050_H
#define LVGL_APP_HAL_MPU6050_H

#include <stdint.h>

class Mpu6050
{
public:
    Mpu6050();
    ~Mpu6050();

    bool init();
    bool update();

    float getRoll() const { return m_roll; }
    float getPitch() const { return m_pitch; }
    float getYaw() const { return m_yaw; }
    float getTemperature() const { return m_temperature; }

private:
    struct KalmanFilter
    {
        float Q_angle;
        float Q_bias;
        float R_measure;
        float angle;
        float bias;
        float P[2][2];
    };

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