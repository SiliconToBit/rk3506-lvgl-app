#include "IoTService.h"
#include "DeviceService.h"
#include "IRCommandManager.h"
#include "IRDevice.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

IoTService& IoTService::instance()
{
    static IoTService instance;
    return instance;
}

IoTService::IoTService()
    : m_commandTopic("rk3506/cmd")
    , m_statusTopic("rk3506/status")
    , m_sensorTopic("rk3506/sensor")
    , m_irStorePath("/data/ir_commands")
    , m_reportRunning(false)
    , m_reportInterval(30)
    , m_initialized(false)
{
    m_cachedSensorData = {0, 0, 0};
}

IoTService::~IoTService()
{
    deinit();
}

bool IoTService::init()
{
    m_mqtt = std::make_unique<MqttService>();

    m_mqtt->setMessageCallback([this](const std::string& topic, const std::string& payload)
                               { handleMqttMessage(topic, payload); });

    m_initialized = true;
    std::cout << "[IoTService] Initialized" << std::endl;
    return true;
}

void IoTService::deinit()
{
    std::cout << "[IoTService] deinit() starting..." << std::endl;

    disableSensorReport();

    disconnectMqtt();
    m_mqtt.reset();

    m_initialized = false;
    std::cout << "[IoTService] Deinitialized" << std::endl;
}

bool IoTService::connectMqtt(const std::string& host, int port, const std::string& clientId)
{
    if (!m_mqtt)
    {
        return false;
    }

    if (!m_mqtt->connect(host, port, clientId))
    {
        std::cerr << "[IoTService] MQTT connection failed" << std::endl;
        return false;
    }

    subscribeMqttTopics();

    std::cout << "[IoTService] MQTT connected to " << host << ":" << port << std::endl;
    return true;
}

void IoTService::disconnectMqtt()
{
    if (m_mqtt)
    {
        m_mqtt->disconnect();
    }
}

bool IoTService::isMqttConnected() const
{
    return m_mqtt && m_mqtt->isConnected();
}

void IoTService::setCommandTopic(const std::string& topic)
{
    m_commandTopic = topic;
}

void IoTService::setStatusTopic(const std::string& topic)
{
    m_statusTopic = topic;
}

void IoTService::setSensorTopic(const std::string& topic)
{
    m_sensorTopic = topic;
}

void IoTService::enableSensorReport(bool enable, int intervalSec)
{
    m_reportInterval = intervalSec;

    if (enable && !m_reportRunning)
    {
        m_reportRunning = true;
        m_reportThread = std::thread(&IoTService::sensorReportThread, this);
        std::cout << "[IoTService] Sensor report enabled, interval: " << intervalSec << "s" << std::endl;
    }
    else if (!enable && m_reportRunning)
    {
        disableSensorReport();
    }
}

void IoTService::disableSensorReport()
{
    m_reportRunning = false;
    if (m_reportThread.joinable())
    {
        m_reportThread.join();
        std::cout << "[IoTService] Sensor report disabled" << std::endl;
    }
}

bool IoTService::isSensorReportEnabled() const
{
    return m_reportRunning;
}

void IoTService::reportSensorData(int temperature, int humidity)
{
    if (!isMqttConnected())
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_sensorDataMutex);
        m_cachedSensorData.temperature = temperature;
        m_cachedSensorData.humidity = humidity;
        m_cachedSensorData.timestamp = std::time(nullptr);
    }

    char payload[256];
    snprintf(payload, sizeof(payload), "{\"temperature\":%d,\"humidity\":%d,\"timestamp\":%ld}", temperature, humidity,
             std::time(nullptr));

    std::string topic = m_sensorTopic + "/dht11";
    m_mqtt->publish(topic, payload);

    std::cout << "[IoTService] Reported sensor data: temp=" << temperature << " humi=" << humidity << std::endl;

    if (m_sensorCallback)
    {
        m_sensorCallback(m_cachedSensorData);
    }
}

void IoTService::sensorReportThread()
{
    while (m_reportRunning)
    {
        // 从DeviceService获取最新传感器数据
        auto& deviceService = DeviceService::instance();
        int temp = deviceService.getTemperature("dht11_1");
        int humi = deviceService.getHumidity("dht11_1");

        if (temp != 0 || humi != 0)
        {
            reportSensorData(temp, humi);
        }

        for (int i = 0; i < m_reportInterval && m_reportRunning; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void IoTService::publishDeviceStatus(const std::string& deviceId, bool state)
{
    if (!isMqttConnected())
    {
        return;
    }

    std::string statusTopic = m_statusTopic + "/" + deviceId;
    std::string payload = state ? "ON" : "OFF";

    m_mqtt->publish(statusTopic, payload);
}

void IoTService::publishDeviceStatus(const std::string& deviceId, const std::string& state)
{
    if (!isMqttConnected())
    {
        return;
    }

    std::string statusTopic = m_statusTopic + "/" + deviceId;
    m_mqtt->publish(statusTopic, state);
}

void IoTService::setSensorDataCallback(SensorDataCallback callback)
{
    m_sensorCallback = std::move(callback);
}

void IoTService::setRemoteCommandCallback(RemoteCommandCallback callback)
{
    m_remoteCommandCallback = std::move(callback);
}

bool IoTService::initIrCommandManager(const std::string& dataPath)
{
    std::string path = dataPath.empty() ? m_irStorePath : dataPath;

    if (!IRCommandManager::getInstance().init(path))
    {
        // 尝试备用路径
        if (!IRCommandManager::getInstance().init("/root/ir_commands"))
        {
            std::cerr << "[IoTService] Warning: IR command manager init failed" << std::endl;
            return false;
        }
        std::cout << "[IoTService] IR command store: /root/ir_commands" << std::endl;
    }
    else
    {
        std::cout << "[IoTService] IR command store: " << path << std::endl;
    }

    // 确保IR设备已打开
    IRDevice& irDevice = IRDevice::getInstance();
    if (!irDevice.isOpen())
    {
        if (irDevice.openDevice())
        {
            std::cout << "[IoTService] IR device opened" << std::endl;
        }
        else
        {
            std::cerr << "[IoTService] Warning: Failed to open IR device" << std::endl;
        }
    }

    return true;
}

void IoTService::setIrCommandStorePath(const std::string& path)
{
    m_irStorePath = path;
}

void IoTService::bindMqttThreadsToCpu(int cpu_id)
{
    if (m_mqtt)
    {
        m_mqtt->bindMqttThreadsToCpu(cpu_id);
    }
}

void IoTService::subscribeMqttTopics()
{
    if (m_mqtt && m_mqtt->isConnected())
    {
        m_mqtt->subscribe(m_commandTopic + "/led01");
        m_mqtt->subscribe(m_commandTopic + "/buzzer01");
        m_mqtt->subscribe(m_commandTopic + "/sensor");
        m_mqtt->subscribe(m_commandTopic + "/ir");
        std::cout << "[IoTService] Subscribed to " << m_commandTopic << "/* topics" << std::endl;
    }
}

void IoTService::handleMqttMessage(const std::string& topic, const std::string& payload)
{
    std::cout << "[IoTService] MQTT [" << topic << "]: " << payload << std::endl;

    // LED 控制: rk3506/cmd/led01
    if (topic == m_commandTopic + "/led01")
    {
        handleLedCommand("led1", payload);
        return;
    }

    // 蜂鸣器控制: rk3506/cmd/buzzer01
    if (topic == m_commandTopic + "/buzzer01")
    {
        handleBuzzerCommand("buzzer1", payload);
        return;
    }

    // 传感器控制: rk3506/cmd/sensor
    if (topic == m_commandTopic + "/sensor")
    {
        handleSensorCommand(payload);
        return;
    }

    // 红外控制: rk3506/cmd/ir
    if (topic == m_commandTopic + "/ir")
    {
        handleIrCommand(payload);
        return;
    }
}

void IoTService::handleLedCommand(const std::string& deviceId, const std::string& command)
{
    bool success = false;
    auto& deviceService = DeviceService::instance();

    if (command == "ON")
    {
        success = deviceService.setDeviceOn(deviceId);
    }
    else if (command == "OFF")
    {
        success = deviceService.setDeviceOff(deviceId);
    }
    else if (command == "TOGGLE")
    {
        success = deviceService.toggleDevice(deviceId);
    }

    if (success)
    {
        publishDeviceStatus(deviceId, deviceService.getDeviceState(deviceId));
    }

    // 调用远程命令回调（如果有）
    if (m_remoteCommandCallback)
    {
        m_remoteCommandCallback("led", deviceId, command);
    }
}

void IoTService::handleBuzzerCommand(const std::string& deviceId, const std::string& command)
{
    bool success = false;
    auto& deviceService = DeviceService::instance();

    if (command == "ON")
    {
        success = deviceService.buzzerOn(deviceId);
    }
    else if (command == "OFF")
    {
        success = deviceService.buzzerOff(deviceId);
    }
    else if (command == "BEEP")
    {
        success = deviceService.buzzerBeep(deviceId, 200);
    }
    else if (command == "ALARM")
    {
        success = deviceService.buzzerBeepPattern(deviceId, 100, 100, 3);
    }

    // 调用远程命令回调（如果有）
    if (m_remoteCommandCallback)
    {
        m_remoteCommandCallback("buzzer", deviceId, command);
    }
}

void IoTService::handleSensorCommand(const std::string& command)
{
    if (command == "DHT11_REPORT")
    {
        auto& deviceService = DeviceService::instance();
        int temp = deviceService.getTemperature("dht11_1");
        int humi = deviceService.getHumidity("dht11_1");
        reportSensorData(temp, humi);
    }

    // 调用远程命令回调（如果有）
    if (m_remoteCommandCallback)
    {
        m_remoteCommandCallback("sensor", "dht11_1", command);
    }
}

void IoTService::handleIrCommand(const std::string& payload)
{
    std::cout << "[IoTService::IR] handleIrCommand: " << payload << std::endl;

    if (payload.empty())
    {
        std::cout << "[IoTService::IR] Error: empty payload" << std::endl;
        publishIrAck(m_commandTopic + "/ir", "", "", "", false, "empty payload");
        return;
    }

    if (payload.front() != '{')
    {
        std::cout << "[IoTService::IR] Error: not json" << std::endl;
        publishIrAck(m_commandTopic + "/ir", "", "", "", false, "invalid json");
        return;
    }

    std::string deviceName;
    std::string cmdName;
    std::string requestId;

    try
    {
        json cmd = json::parse(payload);

        if (cmd.contains("device") && cmd["device"].is_string())
        {
            deviceName = cmd["device"].get<std::string>();
        }
        if (cmd.contains("cmd") && cmd["cmd"].is_string())
        {
            cmdName = cmd["cmd"].get<std::string>();
        }
        if (cmd.contains("request_id") && cmd["request_id"].is_string())
        {
            requestId = cmd["request_id"].get<std::string>();
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "[IoTService::IR] JSON parse error: " << e.what() << std::endl;
        publishIrAck(m_commandTopic + "/ir", requestId, deviceName, cmdName, false, "json parse error");
        return;
    }

    std::cout << "[IoTService::IR] Parsed: device=" << deviceName << ", cmd=" << cmdName << std::endl;

    if (deviceName.empty() || cmdName.empty())
    {
        std::cout << "[IoTService::IR] Error: missing device or cmd" << std::endl;
        publishIrAck(m_commandTopic + "/ir", requestId, deviceName, cmdName, false, "missing device or cmd");
        return;
    }

    std::string mappedDeviceName = mapIrDevice(deviceName);
    std::string irCommandName = resolveIrCommandFromStore(mappedDeviceName, cmdName);

    std::cout << "[IoTService::IR] Mapped: device=" << mappedDeviceName << ", cmd=" << irCommandName << std::endl;

    if (irCommandName.empty())
    {
        std::cout << "[IoTService::IR] Error: unknown cmd" << std::endl;
        publishIrAck(m_commandTopic + "/ir", requestId, deviceName, cmdName, false, "unknown cmd");
        return;
    }

    std::cout << "[IoTService::IR] Emitting: " << mappedDeviceName << " -> " << irCommandName << std::endl;
    bool ok = IRCommandManager::getInstance().emitCommand(mappedDeviceName, irCommandName);
    std::cout << "[IoTService::IR] Emit result: " << (ok ? "success" : "failed") << std::endl;
    publishIrAck(m_commandTopic + "/ir", requestId, deviceName, cmdName, ok, ok ? "success" : "emit failed");

    // 调用远程命令回调（如果有）
    if (m_remoteCommandCallback)
    {
        m_remoteCommandCallback("ir", mappedDeviceName, cmdName);
    }
}

std::string IoTService::mapIrDevice(const std::string& device)
{
    static const std::unordered_map<std::string, std::string> deviceMap = {
        {"AC", "空调"},
    };

    auto it = deviceMap.find(device);
    if (it != deviceMap.end())
    {
        return it->second;
    }
    return device;
}

std::string IoTService::mapIrCommand(const std::string& cmd)
{
    static const std::unordered_map<std::string, std::string> cmdMap = {
        {"POWER_ON", "开"},    {"POWER_OFF", "关"},   {"TEMP_UP", "升温"},
        {"TEMP_DOWN", "降温"}, {"MODE_COOL", "制冷"}, {"MODE_HEAT", "制热"},
    };

    auto it = cmdMap.find(cmd);
    if (it != cmdMap.end())
    {
        return it->second;
    }
    return "";
}

std::vector<std::string> IoTService::buildIrCommandCandidates(const std::string& cmd)
{
    if (cmd == "POWER_ON")
        return {"开", "开机", "电源", "开关"};
    if (cmd == "POWER_OFF")
        return {"关", "关机", "电源", "开关"};
    if (cmd == "TEMP_UP")
        return {"温度+", "升温", "加温", "温度上调"};
    if (cmd == "TEMP_DOWN")
        return {"温度-", "降温", "减温", "温度下调"};
    if (cmd == "MODE_COOL")
        return {"制冷", "模式-制冷", "冷风"};
    if (cmd == "MODE_HEAT")
        return {"制热", "模式-制热", "暖风"};
    return {};
}

std::string IoTService::resolveIrCommandFromStore(const std::string& deviceName, const std::string& cmd)
{
    auto candidates = buildIrCommandCandidates(cmd);
    for (const auto& candidate : candidates)
    {
        if (IRCommandManager::getInstance().commandExists(deviceName, candidate))
        {
            return candidate;
        }
    }

    // 没有候选命中时，返回兼容旧逻辑的默认映射
    if (cmd == "POWER_ON")
        return "开";
    if (cmd == "POWER_OFF")
        return "关";
    if (cmd == "TEMP_UP")
        return "升温";
    if (cmd == "TEMP_DOWN")
        return "降温";
    if (cmd == "MODE_COOL")
        return "制冷";
    if (cmd == "MODE_HEAT")
        return "制热";
    return "";
}

std::string IoTService::buildIrAckTopic(const std::string& commandTopic)
{
    const std::string suffix = "/cmd";
    if (commandTopic.size() >= suffix.size() &&
        commandTopic.compare(commandTopic.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        return commandTopic.substr(0, commandTopic.size() - suffix.size()) + "/ack";
    }
    return commandTopic + "/ack";
}

void IoTService::publishIrAck(const std::string& commandTopic, const std::string& requestId, const std::string& device,
                              const std::string& command, bool success, const std::string& message)
{
    if (!isMqttConnected())
    {
        return;
    }

    const std::string ackTopic = buildIrAckTopic(commandTopic);
    json ack = {
        {"request_id", requestId}, {"ok", success}, {"device", device}, {"command", command}, {"msg", message},
    };
    m_mqtt->publish(ackTopic, ack.dump());
}
