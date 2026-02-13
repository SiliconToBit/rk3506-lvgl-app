/**
 * @file DeviceService.h
 * @brief 设备管理服务类
 * @details 管理LED、蜂鸣器等设备的本地控制和MQTT远程控制，支持温湿度上报
 */

#ifndef LVGL_APP_SERVICE_DEVICE_SERVICE_H
#define LVGL_APP_SERVICE_DEVICE_SERVICE_H

#include "Buzzer.h"
#include "Dht11.h"
#include "Led.h"
#include "MqttService.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <atomic>

struct DeviceInfo
{
    std::string name;
    std::string type;
    bool state;
};

struct SensorData
{
    int temperature;
    int humidity;
    long timestamp;
};

class DeviceService
{
public:
    using DeviceStateCallback = std::function<void(const std::string& deviceId, bool state)>;
    using SensorDataCallback = std::function<void(const SensorData& data)>;

    static DeviceService& instance();

    DeviceService(const DeviceService&) = delete;
    DeviceService& operator=(const DeviceService&) = delete;

    bool init();
    void deinit();

    bool addLed(const std::string& deviceId, const std::string& gpioPath);
    bool setDeviceOn(const std::string& deviceId);
    bool setDeviceOff(const std::string& deviceId);
    bool toggleDevice(const std::string& deviceId);
    bool getDeviceState(const std::string& deviceId) const;
    bool hasDevice(const std::string& deviceId) const;

    bool addBuzzer(const std::string& deviceId, const std::string& gpioPath);
    bool buzzerOn(const std::string& deviceId);
    bool buzzerOff(const std::string& deviceId);
    bool buzzerBeep(const std::string& deviceId, int durationMs);
    bool buzzerBeepPattern(const std::string& deviceId, int onMs, int offMs, int count);

    bool addDht11(const std::string& deviceId, const std::string& devPath);
    SensorData getSensorData(const std::string& deviceId) const;
    int getTemperature(const std::string& deviceId);
    int getHumidity(const std::string& deviceId);

    void enableSensorReport(bool enable, int intervalSec = 30);
    void setSensorReportTopic(const std::string& topic);
    void reportSensorData();

    bool removeDevice(const std::string& deviceId);
    std::map<std::string, DeviceInfo> getAllDevices() const;
    void setDeviceStateCallback(DeviceStateCallback callback);
    void setSensorDataCallback(SensorDataCallback callback);

    bool connectMqtt(const std::string& host, int port, const std::string& clientId = "device_controller");
    void disconnectMqtt();
    bool isMqttConnected() const;

    void setMqttCommandTopic(const std::string& topic);
    void setMqttStatusTopic(const std::string& topic);

private:
    DeviceService();
    ~DeviceService();

    void handleMqttMessage(const std::string& topic, const std::string& payload);
    void publishDeviceStatus(const std::string& deviceId, bool state);
    void subscribeMqttTopics();
    void sensorReportThread();

    std::map<std::string, std::unique_ptr<Led>> m_leds;
    std::map<std::string, std::unique_ptr<Buzzer>> m_buzzers;
    std::map<std::string, std::unique_ptr<Dht11>> m_dht11s;
    std::map<std::string, DeviceInfo> m_deviceInfos;
    std::map<std::string, SensorData> m_sensorDatas;

    std::unique_ptr<MqttService> m_mqtt;
    std::string m_commandTopic;
    std::string m_statusTopic;
    std::string m_sensorTopic;
    DeviceStateCallback m_stateCallback;
    SensorDataCallback m_sensorCallback;

    std::thread m_reportThread;
    std::atomic<bool> m_reportRunning;
    int m_reportInterval;
    bool m_initialized;
};

#endif
