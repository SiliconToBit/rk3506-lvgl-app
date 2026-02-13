#include "DeviceService.h"
#include <ctime>
#include <iostream>
#include <chrono>

DeviceService& DeviceService::instance()
{
    static DeviceService instance;
    return instance;
}

DeviceService::DeviceService()
    : m_commandTopic("device/command")
    , m_statusTopic("device/status")
    , m_sensorTopic("sensor/data")
    , m_reportRunning(false)
    , m_reportInterval(30)
    , m_initialized(false)
{
}

DeviceService::~DeviceService()
{
    deinit();
}

bool DeviceService::init()
{
    m_mqtt = std::make_unique<MqttService>();

    m_mqtt->setMessageCallback([this](const std::string& topic, const std::string& payload) {
        handleMqttMessage(topic, payload);
    });

    m_initialized = true;
    std::cout << "[DeviceService] Initialized" << std::endl;
    return true;
}

void DeviceService::deinit()
{
    m_reportRunning = false;
    if (m_reportThread.joinable())
    {
        m_reportThread.join();
    }

    m_leds.clear();
    m_buzzers.clear();
    m_dht11s.clear();
    m_deviceInfos.clear();
    m_sensorDatas.clear();
    m_mqtt.reset();
    m_initialized = false;
    std::cout << "[DeviceService] Deinitialized" << std::endl;
}

bool DeviceService::addLed(const std::string& deviceId, const std::string& gpioPath)
{
    if (m_leds.find(deviceId) != m_leds.end())
    {
        std::cerr << "[DeviceService] LED already exists: " << deviceId << std::endl;
        return false;
    }

    auto led = std::make_unique<Led>(gpioPath);
    if (!led->open())
    {
        std::cerr << "[DeviceService] Failed to open LED: " << deviceId << std::endl;
        return false;
    }

    DeviceInfo info;
    info.name = deviceId;
    info.type = "led";
    info.state = false;

    m_deviceInfos[deviceId] = info;
    m_leds[deviceId] = std::move(led);

    std::cout << "[DeviceService] Added LED: " << deviceId << " -> " << gpioPath << std::endl;
    return true;
}

bool DeviceService::setDeviceOn(const std::string& deviceId)
{
    auto it = m_leds.find(deviceId);
    if (it == m_leds.end())
    {
        std::cerr << "[DeviceService] LED not found: " << deviceId << std::endl;
        return false;
    }

    if (!it->second->setOn())
    {
        return false;
    }

    m_deviceInfos[deviceId].state = true;

    if (m_stateCallback)
    {
        m_stateCallback(deviceId, true);
    }

    publishDeviceStatus(deviceId, true);

    std::cout << "[DeviceService] " << deviceId << " turned ON" << std::endl;
    return true;
}

bool DeviceService::setDeviceOff(const std::string& deviceId)
{
    auto it = m_leds.find(deviceId);
    if (it == m_leds.end())
    {
        std::cerr << "[DeviceService] LED not found: " << deviceId << std::endl;
        return false;
    }

    if (!it->second->setOff())
    {
        return false;
    }

    m_deviceInfos[deviceId].state = false;

    if (m_stateCallback)
    {
        m_stateCallback(deviceId, false);
    }

    publishDeviceStatus(deviceId, false);

    std::cout << "[DeviceService] " << deviceId << " turned OFF" << std::endl;
    return true;
}

bool DeviceService::toggleDevice(const std::string& deviceId)
{
    bool currentState = getDeviceState(deviceId);
    return currentState ? setDeviceOff(deviceId) : setDeviceOn(deviceId);
}

bool DeviceService::getDeviceState(const std::string& deviceId) const
{
    auto it = m_deviceInfos.find(deviceId);
    if (it == m_deviceInfos.end())
    {
        return false;
    }
    return it->second.state;
}

bool DeviceService::hasDevice(const std::string& deviceId) const
{
    return m_deviceInfos.find(deviceId) != m_deviceInfos.end();
}

// 蜂鸣器控制
bool DeviceService::addBuzzer(const std::string& deviceId, const std::string& gpioPath)
{
    if (m_buzzers.find(deviceId) != m_buzzers.end())
    {
        std::cerr << "[DeviceService] Buzzer already exists: " << deviceId << std::endl;
        return false;
    }

    auto buzzer = std::make_unique<Buzzer>(gpioPath);
    if (!buzzer->open())
    {
        std::cerr << "[DeviceService] Failed to open Buzzer: " << deviceId << std::endl;
        return false;
    }

    DeviceInfo info;
    info.name = deviceId;
    info.type = "buzzer";
    info.state = false;

    m_deviceInfos[deviceId] = info;
    m_buzzers[deviceId] = std::move(buzzer);

    std::cout << "[DeviceService] Added Buzzer: " << deviceId << " -> " << gpioPath << std::endl;
    return true;
}

bool DeviceService::buzzerOn(const std::string& deviceId)
{
    auto it = m_buzzers.find(deviceId);
    if (it == m_buzzers.end())
    {
        std::cerr << "[DeviceService] Buzzer not found: " << deviceId << std::endl;
        return false;
    }

    bool result = it->second->setOn();
    if (result)
    {
        m_deviceInfos[deviceId].state = true;
        std::cout << "[DeviceService] " << deviceId << " buzzer ON" << std::endl;
    }
    return result;
}

bool DeviceService::buzzerOff(const std::string& deviceId)
{
    auto it = m_buzzers.find(deviceId);
    if (it == m_buzzers.end())
    {
        std::cerr << "[DeviceService] Buzzer not found: " << deviceId << std::endl;
        return false;
    }

    bool result = it->second->setOff();
    if (result)
    {
        m_deviceInfos[deviceId].state = false;
        std::cout << "[DeviceService] " << deviceId << " buzzer OFF" << std::endl;
    }
    return result;
}

bool DeviceService::buzzerBeep(const std::string& deviceId, int durationMs)
{
    auto it = m_buzzers.find(deviceId);
    if (it == m_buzzers.end())
    {
        std::cerr << "[DeviceService] Buzzer not found: " << deviceId << std::endl;
        return false;
    }

    std::cout << "[DeviceService] " << deviceId << " beep " << durationMs << "ms" << std::endl;
    return it->second->beep(durationMs);
}

bool DeviceService::buzzerBeepPattern(const std::string& deviceId, int onMs, int offMs, int count)
{
    auto it = m_buzzers.find(deviceId);
    if (it == m_buzzers.end())
    {
        std::cerr << "[DeviceService] Buzzer not found: " << deviceId << std::endl;
        return false;
    }

    std::cout << "[DeviceService] " << deviceId << " beep pattern: " << onMs << "/" << offMs << " x " << count << std::endl;
    return it->second->beepPattern(onMs, offMs, count);
}

// 温湿度传感器
bool DeviceService::addDht11(const std::string& deviceId, const std::string& devPath)
{
    if (m_dht11s.find(deviceId) != m_dht11s.end())
    {
        std::cerr << "[DeviceService] DHT11 already exists: " << deviceId << std::endl;
        return false;
    }

    auto dht11 = std::make_unique<Dht11>(devPath);
    if (!dht11->open())
    {
        std::cerr << "[DeviceService] Failed to open DHT11: " << deviceId << std::endl;
        return false;
    }

    DeviceInfo info;
    info.name = deviceId;
    info.type = "dht11";
    info.state = true;

    SensorData sensorData;
    sensorData.temperature = 0;
    sensorData.humidity = 0;
    sensorData.timestamp = 0;

    m_deviceInfos[deviceId] = info;
    m_sensorDatas[deviceId] = sensorData;
    m_dht11s[deviceId] = std::move(dht11);

    std::cout << "[DeviceService] Added DHT11: " << deviceId << " -> " << devPath << std::endl;
    return true;
}

SensorData DeviceService::getSensorData(const std::string& deviceId) const
{
    auto it = m_sensorDatas.find(deviceId);
    if (it == m_sensorDatas.end())
    {
        return SensorData{0, 0, 0};
    }
    return it->second;
}

int DeviceService::getTemperature(const std::string& deviceId)
{
    auto it = m_dht11s.find(deviceId);
    if (it == m_dht11s.end())
    {
        return 0;
    }

    int temp = it->second->readTemperature();

    auto dataIt = m_sensorDatas.find(deviceId);
    if (dataIt != m_sensorDatas.end())
    {
        dataIt->second.temperature = temp;
        dataIt->second.timestamp = std::time(nullptr);
    }

    return temp;
}

int DeviceService::getHumidity(const std::string& deviceId)
{
    auto it = m_dht11s.find(deviceId);
    if (it == m_dht11s.end())
    {
        return 0;
    }

    int humi = it->second->readHumidity();

    auto dataIt = m_sensorDatas.find(deviceId);
    if (dataIt != m_sensorDatas.end())
    {
        dataIt->second.humidity = humi;
        dataIt->second.timestamp = std::time(nullptr);
    }

    return humi;
}

// 温湿度上报
void DeviceService::enableSensorReport(bool enable, int intervalSec)
{
    m_reportInterval = intervalSec;

    if (enable && !m_reportRunning)
    {
        m_reportRunning = true;
        m_reportThread = std::thread(&DeviceService::sensorReportThread, this);
        std::cout << "[DeviceService] Sensor report enabled, interval: " << intervalSec << "s" << std::endl;
    }
    else if (!enable && m_reportRunning)
    {
        m_reportRunning = false;
        if (m_reportThread.joinable())
        {
            m_reportThread.join();
        }
        std::cout << "[DeviceService] Sensor report disabled" << std::endl;
    }
}

void DeviceService::setSensorReportTopic(const std::string& topic)
{
    m_sensorTopic = topic;
}

void DeviceService::reportSensorData()
{
    if (!isMqttConnected())
    {
        return;
    }

    for (const auto& pair : m_dht11s)
    {
        const std::string& deviceId = pair.first;
        int temp = getTemperature(deviceId);
        int humi = getHumidity(deviceId);

        char payload[256];
        snprintf(payload, sizeof(payload),
                 "{\"device\":\"%s\",\"temperature\":%d,\"humidity\":%d,\"timestamp\":%ld}",
                 deviceId.c_str(), temp, humi, std::time(nullptr));

        std::string topic = m_sensorTopic + "/" + deviceId;
        m_mqtt->publish(topic, payload);

        std::cout << "[DeviceService] Reported sensor data: " << deviceId
                  << " temp=" << temp << " humi=" << humi << std::endl;
    }
}

void DeviceService::sensorReportThread()
{
    while (m_reportRunning)
    {
        reportSensorData();

        for (int i = 0; i < m_reportInterval && m_reportRunning; ++i)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

// 设备管理
bool DeviceService::removeDevice(const std::string& deviceId)
{
    bool removed = false;

    if (m_leds.erase(deviceId) > 0)
    {
        removed = true;
    }
    if (m_buzzers.erase(deviceId) > 0)
    {
        removed = true;
    }
    if (m_dht11s.erase(deviceId) > 0)
    {
        removed = true;
        m_sensorDatas.erase(deviceId);
    }

    m_deviceInfos.erase(deviceId);

    if (removed)
    {
        std::cout << "[DeviceService] Removed device: " << deviceId << std::endl;
    }

    return removed;
}

std::map<std::string, DeviceInfo> DeviceService::getAllDevices() const
{
    return m_deviceInfos;
}

void DeviceService::setDeviceStateCallback(DeviceStateCallback callback)
{
    m_stateCallback = std::move(callback);
}

void DeviceService::setSensorDataCallback(SensorDataCallback callback)
{
    m_sensorCallback = std::move(callback);
}

// MQTT 连接
bool DeviceService::connectMqtt(const std::string& host, int port, const std::string& clientId)
{
    if (!m_mqtt)
    {
        return false;
    }

    if (!m_mqtt->connect(host, port, clientId))
    {
        std::cerr << "[DeviceService] MQTT connection failed" << std::endl;
        return false;
    }

    subscribeMqttTopics();

    std::cout << "[DeviceService] MQTT connected to " << host << ":" << port << std::endl;
    return true;
}

void DeviceService::disconnectMqtt()
{
    if (m_mqtt)
    {
        m_mqtt->disconnect();
    }
}

bool DeviceService::isMqttConnected() const
{
    return m_mqtt && m_mqtt->isConnected();
}

void DeviceService::setMqttCommandTopic(const std::string& topic)
{
    m_commandTopic = topic;
}

void DeviceService::setMqttStatusTopic(const std::string& topic)
{
    m_statusTopic = topic;
}

void DeviceService::handleMqttMessage(const std::string& topic, const std::string& payload)
{
    if (topic != m_commandTopic)
    {
        return;
    }

    std::cout << "[DeviceService] MQTT command: " << payload << std::endl;

    // LED 命令
    if (payload == "led_on")
    {
        setDeviceOn("led1");
    }
    else if (payload == "led_off")
    {
        setDeviceOff("led1");
    }
    else if (payload == "led_toggle")
    {
        toggleDevice("led1");
    }
    // 蜂鸣器命令
    else if (payload == "buzzer_on")
    {
        buzzerOn("buzzer1");
    }
    else if (payload == "buzzer_off")
    {
        buzzerOff("buzzer1");
    }
    else if (payload == "buzzer_beep")
    {
        buzzerBeep("buzzer1", 200);
    }
    else if (payload == "buzzer_alarm")
    {
        buzzerBeepPattern("buzzer1", 100, 100, 3);
    }
    // 传感器命令
    else if (payload == "sensor_report")
    {
        reportSensorData();
    }
    // 通用命令格式: set:deviceId:action
    else if (payload.find("set:") == 0)
    {
        size_t colonPos1 = payload.find(':', 4);
        if (colonPos1 != std::string::npos)
        {
            std::string deviceId = payload.substr(4, colonPos1 - 4);
            std::string action = payload.substr(colonPos1 + 1);

            if (action == "on")
            {
                if (m_leds.find(deviceId) != m_leds.end())
                {
                    setDeviceOn(deviceId);
                }
                else if (m_buzzers.find(deviceId) != m_buzzers.end())
                {
                    buzzerOn(deviceId);
                }
            }
            else if (action == "off")
            {
                if (m_leds.find(deviceId) != m_leds.end())
                {
                    setDeviceOff(deviceId);
                }
                else if (m_buzzers.find(deviceId) != m_buzzers.end())
                {
                    buzzerOff(deviceId);
                }
            }
            else if (action == "toggle")
            {
                toggleDevice(deviceId);
            }
        }
    }
}

void DeviceService::publishDeviceStatus(const std::string& deviceId, bool state)
{
    if (!isMqttConnected())
    {
        return;
    }

    std::string statusTopic = m_statusTopic + "/" + deviceId;
    std::string payload = state ? "on" : "off";

    m_mqtt->publish(statusTopic, payload);
}

void DeviceService::subscribeMqttTopics()
{
    if (m_mqtt && m_mqtt->isConnected())
    {
        m_mqtt->subscribe(m_commandTopic);
        std::cout << "[DeviceService] Subscribed to: " << m_commandTopic << std::endl;
    }
}
