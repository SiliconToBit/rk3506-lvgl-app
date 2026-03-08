/**
 * @file IoTService.h
 * @brief IoT服务管理类
 * @details 管理MQTT通信、传感器数据上报、远程设备控制
 */

#ifndef LVGL_APP_SERVICE_IOT_SERVICE_H
#define LVGL_APP_SERVICE_IOT_SERVICE_H

#include "MqttService.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <unordered_map>

// 前向声明
class DeviceService;

struct SensorData
{
    int temperature;
    int humidity;
    long timestamp;
};

class IoTService
{
public:
    using SensorDataCallback = std::function<void(const SensorData& data)>;
    using RemoteCommandCallback =
        std::function<bool(const std::string& deviceType, const std::string& deviceId, const std::string& command)>;

    static IoTService& instance();

    IoTService(const IoTService&) = delete;
    IoTService& operator=(const IoTService&) = delete;

    // 初始化和反初始化
    bool init();
    void deinit();

    // MQTT连接管理
    bool connectMqtt(const std::string& host, int port, const std::string& clientId = "rk3506_iot");
    void disconnectMqtt();
    bool isMqttConnected() const;

    // MQTT主题设置
    void setCommandTopic(const std::string& topic);
    void setStatusTopic(const std::string& topic);
    void setSensorTopic(const std::string& topic);

    // 传感器数据上报
    void enableSensorReport(bool enable, int intervalSec = 30);
    void disableSensorReport();
    bool isSensorReportEnabled() const;
    void reportSensorData(int temperature, int humidity);

    // 设备状态发布
    void publishDeviceStatus(const std::string& deviceId, bool state);
    void publishDeviceStatus(const std::string& deviceId, const std::string& state);

    // 回调设置
    void setSensorDataCallback(SensorDataCallback callback);
    void setRemoteCommandCallback(RemoteCommandCallback callback);

    // IR控制相关
    void setIrCommandStorePath(const std::string& path);
    bool initIrCommandManager(const std::string& dataPath);

    // 绑定 MQTT 线程到指定 CPU
    void bindMqttThreadsToCpu(int cpu_id);

private:
    IoTService();
    ~IoTService();

    // MQTT消息处理
    void handleMqttMessage(const std::string& topic, const std::string& payload);
    void subscribeMqttTopics();

    // 命令处理
    void handleLedCommand(const std::string& deviceId, const std::string& command);
    void handleBuzzerCommand(const std::string& deviceId, const std::string& command);
    void handleSensorCommand(const std::string& command);
    void handleIrCommand(const std::string& payload);

    // IR相关辅助函数
    std::string mapIrDevice(const std::string& device);
    std::string mapIrCommand(const std::string& cmd);
    std::string resolveIrCommandFromStore(const std::string& deviceName, const std::string& cmd);
    std::vector<std::string> buildIrCommandCandidates(const std::string& cmd);
    void publishIrAck(const std::string& commandTopic, const std::string& requestId, const std::string& device,
                      const std::string& command, bool success, const std::string& message);
    std::string buildIrAckTopic(const std::string& commandTopic);

    // 上报线程
    void sensorReportThread();

    std::unique_ptr<MqttService> m_mqtt;
    std::string m_commandTopic;
    std::string m_statusTopic;
    std::string m_sensorTopic;
    std::string m_irStorePath;

    SensorDataCallback m_sensorCallback;
    RemoteCommandCallback m_remoteCommandCallback;

    std::thread m_reportThread;
    std::atomic<bool> m_reportRunning;
    int m_reportInterval;
    bool m_initialized;

    // 缓存的传感器数据
    SensorData m_cachedSensorData;
    std::mutex m_sensorDataMutex;
};

#endif
