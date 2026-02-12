#include "Mpu6050.h"
#include "AppConfig.h"
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define RAD_TO_DEG 57.295779513082320876

Mpu6050::Mpu6050()
    : m_fd(-1), m_roll(0), m_pitch(0), m_yaw(0), m_temperature(0), m_lastTime(0), m_gyroBiasX(0), m_gyroBiasY(0),
      m_gyroBiasZ(0)
{
    kalmanInit(&m_kalmanX);
    kalmanInit(&m_kalmanY);
}

Mpu6050::~Mpu6050()
{
    if (m_fd >= 0)
    {
        close(m_fd);
    }
}

bool Mpu6050::init()
{
    m_fd = open(APP_DEV_MPU6050, O_RDONLY);
    if (m_fd < 0)
    {
        perror("Mpu6050 Open failed");
        return false;
    }
    calibrate();
    m_lastTime = getTimeSec();
    return true;
}

void Mpu6050::calibrate()
{
    printf("Keep sensor still! Calibrating gyro...\n");
    long gx_sum = 0, gy_sum = 0, gz_sum = 0;
    const int CALIB_COUNT = 500;
    struct RawData raw;

    for (int i = 0; i < CALIB_COUNT; i++)
    {
        if (read(m_fd, &raw, sizeof(raw)) == sizeof(raw))
        {
            gx_sum += (short)((raw.gyro_x_h << 8) | raw.gyro_x_l);
            gy_sum += (short)((raw.gyro_y_h << 8) | raw.gyro_y_l);
            gz_sum += (short)((raw.gyro_z_h << 8) | raw.gyro_z_l);
            usleep(2000);
        }
    }

    m_gyroBiasX = (float)gx_sum / CALIB_COUNT / 131.0f;
    m_gyroBiasY = (float)gy_sum / CALIB_COUNT / 131.0f;
    m_gyroBiasZ = (float)gz_sum / CALIB_COUNT / 131.0f;
    printf("Calibration Done! Bias X:%.3f Y:%.3f Z:%.3f\n", m_gyroBiasX, m_gyroBiasY, m_gyroBiasZ);
}

double Mpu6050::getTimeSec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void Mpu6050::kalmanInit(KalmanFilter *k)
{
    k->Q_angle = 0.001f;
    k->Q_bias = 0.003f;
    k->R_measure = 0.03f;

    k->angle = 0.0f;
    k->bias = 0.0f;

    k->P[0][0] = 0.0f;
    k->P[0][1] = 0.0f;
    k->P[1][0] = 0.0f;
    k->P[1][1] = 0.0f;
}

// 卡尔曼核心算法
float Mpu6050::kalmanGetAngle(KalmanFilter *k, float newAngle, float newRate, float dt)
{
    // 1. 预测 (Predict)
    float rate = newRate - k->bias;
    k->angle += dt * rate;

    k->P[0][0] += dt * (dt * k->P[1][1] - k->P[0][1] - k->P[1][0] + k->Q_angle);
    k->P[0][1] -= dt * k->P[1][1];
    k->P[1][0] -= dt * k->P[1][1];
    k->P[1][1] += k->Q_bias * dt;

    // 2. 更新 (Update)
    float S = k->P[0][0] + k->R_measure; // 创新协方差
    float K[2];                          // 卡尔曼增益
    K[0] = k->P[0][0] / S;
    K[1] = k->P[1][0] / S;

    float y = newAngle - k->angle; // 创新 (Innovation)

    k->angle += K[0] * y;
    k->bias += K[1] * y;

    float P00_temp = k->P[0][0];
    float P01_temp = k->P[0][1];

    k->P[0][0] -= K[0] * P00_temp;
    k->P[0][1] -= K[0] * P01_temp;
    k->P[1][0] -= K[1] * P00_temp;
    k->P[1][1] -= K[1] * P01_temp;

    return k->angle;
}

bool Mpu6050::update()
{
    if (m_fd < 0)
        return false;

    struct RawData raw;
    // 尝试读取一次数据，非阻塞调用者
    if (read(m_fd, &raw, sizeof(raw)) != sizeof(raw))
    {
        return false;
    }

    double current_time = getTimeSec();
    double dt = current_time - m_lastTime;
    m_lastTime = current_time;
    if (dt <= 0)
        dt = 0.01; // 防止除零或负数

    // 数据解析
    short ax_raw = (raw.accel_x_h << 8) | raw.accel_x_l;
    short ay_raw = (raw.accel_y_h << 8) | raw.accel_y_l;
    short az_raw = (raw.accel_z_h << 8) | raw.accel_z_l;
    short gx_raw = (raw.gyro_x_h << 8) | raw.gyro_x_l;
    short gy_raw = (raw.gyro_y_h << 8) | raw.gyro_y_l;
    short gz_raw = (raw.gyro_z_h << 8) | raw.gyro_z_l;
    short temp_raw = (raw.temp_h << 8) | raw.temp_l;

    // 物理单位转换 & 减去零点偏差
    float ax = ax_raw / 16384.0f;
    float ay = ay_raw / 16384.0f;
    float az = az_raw / 16384.0f;
    float gx = (gx_raw / 131.0f) - m_gyroBiasX;
    float gy = (gy_raw / 131.0f) - m_gyroBiasY;
    float gz = (gz_raw / 131.0f) - m_gyroBiasZ;

    m_temperature = temp_raw / 340.0f + 36.53f;

    // --- 死区设置 (防止静止时的微小漂移) ---
    if (fabs(gx) < 0.05)
        gx = 0;
    if (fabs(gy) < 0.05)
        gy = 0;
    if (fabs(gz) < 0.05)
        gz = 0; // 最关键的 Yaw 轴死区

    // --- 卡尔曼滤波步骤 ---

    // 1. 计算加速度计的角度 (观测值)
    float acc_roll = atan2(ay, az) * RAD_TO_DEG;
    float acc_pitch = atan2(-ax, sqrt(ay * ay + az * az)) * RAD_TO_DEG;

    // 2. 传入卡尔曼滤波器
    m_roll = kalmanGetAngle(&m_kalmanX, acc_roll, gx, dt);
    m_pitch = kalmanGetAngle(&m_kalmanY, acc_pitch, gy, dt);

    return true;
}