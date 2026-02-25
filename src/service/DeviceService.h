/**
 * @file DeviceService.h
 * @brief 硬件设备管理服务类
 * @details 管理LED、蜂鸣器、DHT11传感器等硬件设备的本地控制
 */

#ifndef LVGL_APP_SERVICE_DEVICE_SERVICE_H
#define LVGL_APP_SERVICE_DEVICE_SERVICE_H

#include "Buzzer.h"
#include "Dht11.h"
#include "Led.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <atomic>

struct DeviceInfo
{
    std::string name;
    std::string type;
    bool state;
};

// SensorData 定义在 IoTService.h 中，这里使用前向声明
struct SensorData;

class DeviceService
{
public:
    using DeviceStateCallback = std::function<void(const std::string& deviceId, bool state)>;
    using SensorDataCallback = std::function<void(const SensorData& data)>;

    static DeviceService& instance();

    DeviceService(const DeviceService&) = delete;
    DeviceService& operator=(const DeviceService&) = delete;

    // 初始化和反初始化
    bool init();
    void deinit();
    bool isInitialized() const
    {
        return m_initialized;
    }

    // LED设备管理
    bool addLed(const std::string& deviceId, const std::string& gpioPath);
    bool setDeviceOn(const std::string& deviceId);
    bool setDeviceOff(const std::string& deviceId);
    bool toggleDevice(const std::string& deviceId);
    bool getDeviceState(const std::string& deviceId) const;
    bool hasDevice(const std::string& deviceId) const;

    // 蜂鸣器设备管理
    bool addBuzzer(const std::string& deviceId, const std::string& gpioPath);
    bool buzzerOn(const std::string& deviceId);
    bool buzzerOff(const std::string& deviceId);
    bool buzzerBeep(const std::string& deviceId, int durationMs);
    bool buzzerBeepPattern(const std::string& deviceId, int onMs, int offMs, int count);

    // DHT11传感器管理
    bool addDht11(const std::string& deviceId, const std::string& devPath);
    SensorData getSensorData(const std::string& deviceId) const;
    int getTemperature(const std::string& deviceId);
    int getHumidity(const std::string& deviceId);

    // 设备管理
    bool removeDevice(const std::string& deviceId);
    std::map<std::string, DeviceInfo> getAllDevices() const;

    // 回调设置
    void setDeviceStateCallback(DeviceStateCallback callback);
    void setSensorDataCallback(SensorDataCallback callback);

private:
    DeviceService();
    ~DeviceService();

    std::map<std::string, std::unique_ptr<Led>> m_leds;
    std::map<std::string, std::unique_ptr<Buzzer>> m_buzzers;
    std::map<std::string, std::unique_ptr<Dht11>> m_dht11s;
    std::map<std::string, DeviceInfo> m_deviceInfos;
    std::map<std::string, SensorData> m_sensorDatas;

    DeviceStateCallback m_stateCallback;
    SensorDataCallback m_sensorCallback;

    bool m_initialized;
};

#endif
