/**
 * @file AppConfig.h
 * @brief 应用配置管理类
 * @details 负责应用的硬件设备路径、应用配置和字体路径的定义
 */

#ifndef LVGL_APP_CONFIG_H
#define LVGL_APP_CONFIG_H

/* ======================== 硬件设备路径 ======================== */
#define APP_DEV_DHT11 "/dev/dht11"
#define APP_DEV_MPU6050 "/dev/mpu6050"
#define APP_DEV_BACKLIGHT "/sys/class/backlight/backlight/"

/* ======================== GPIO 设备路径 ======================= */
#define APP_GPIO_LED1 "/dev/led1"
#define APP_GPIO_LED2 "/dev/led2"
#define APP_GPIO_BUZZER "/dev/buzzer"

/* ======================== MQTT 配置 =========================== */
#define APP_MQTT_HOST "192.168.1.100"
#define APP_MQTT_PORT 1883
#define APP_MQTT_CLIENT_ID "rk3506_device"

/* ======================== 传感器上报配置 ====================== */
#define APP_SENSOR_REPORT_INTERVAL 30

/* ======================== 应用配置 ============================ */
#define APP_MUSIC_DIR "/root/Music"
#define APP_WEATHER_API_KEY "4661897787bc49c793207316caf28304"
#define APP_DEFAULT_CITY "广州"

/* ======================== 字体路径 ============================ */
#define APP_FONT_PATH "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf"
#define APP_WEATHER_FONT_PATH "/usr/share/fonts/qweather-icons.ttf"

#endif
