#include "DeviceService.h"
#include "IRCommandManager.h"
#include "IRDevice.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace
{
std::string buildIrAckTopic(const std::string &commandTopic)
{
    const std::string suffix = "/cmd";
    if (commandTopic.size() >= suffix.size() &&
        commandTopic.compare(commandTopic.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        return commandTopic.substr(0, commandTopic.size() - suffix.size()) + "/ack";
    }
    return commandTopic + "/ack";
}

void publishIrAck(MqttService *mqtt,
                  const std::string &commandTopic,
                  const std::string &requestId,
                  const std::string &device,
                  const std::string &command,
                  bool success,
                  const std::string &message)
{
    if (!mqtt || !mqtt->isConnected())
    {
        return;
    }

    const std::string ackTopic = buildIrAckTopic(commandTopic);
    json ack = {
        {"request_id", requestId},
        {"ok", success},
        {"device", device},
        {"command", command},
        {"msg", message},
    };
    mqtt->publish(ackTopic, ack.dump());
}

std::vector<std::string> buildIrCommandCandidates(const std::string &cmd)
{
    if (cmd == "POWER_ON") return {"开", "开机", "电源", "开关"};
    if (cmd == "POWER_OFF") return {"关", "关机", "电源", "开关"};
    if (cmd == "TEMP_UP") return {"温度+", "升温", "加温", "温度上调"};
    if (cmd == "TEMP_DOWN") return {"温度-", "降温", "减温", "温度下调"};
    if (cmd == "MODE_COOL") return {"制冷", "模式-制冷", "冷风"};
    if (cmd == "MODE_HEAT") return {"制热", "模式-制热", "暖风"};
    return {};
}

std::string resolveIrCommandFromStore(const std::string &deviceName, const std::string &cmd)
{
    auto candidates = buildIrCommandCandidates(cmd);
    for (const auto &candidate : candidates)
    {
        if (IRCommandManager::getInstance().commandExists(deviceName, candidate))
        {
            return candidate;
        }
    }

    // 没有候选命中时，返回兼容旧逻辑的默认映射
    if (cmd == "POWER_ON") return "开";
    if (cmd == "POWER_OFF") return "关";
    if (cmd == "TEMP_UP") return "升温";
    if (cmd == "TEMP_DOWN") return "降温";
    if (cmd == "MODE_COOL") return "制冷";
    if (cmd == "MODE_HEAT") return "制热";
    return "";
}
}

/**
 * @brief 获取单例实例
 * @return DeviceService& 单例引用
 * @details 使用静态局部变量实现线程安全的单例模式(C++11)
 */
DeviceService& DeviceService::instance()
{
    static DeviceService instance;
    return instance;
}

/**
 * @brief 构造函数
 * @details 初始化MQTT主题、上报间隔等默认配置
 */
DeviceService::DeviceService()
    : m_commandTopic("rk3506/cmd")
    , m_statusTopic("rk3506/status")
    , m_sensorTopic("rk3506/sensor")
    , m_reportRunning(false)
    , m_reportInterval(30)
    , m_initialized(false)
{
}

/**
 * @brief 析构函数
 * @details 自动调用deinit()释放资源
 */
DeviceService::~DeviceService()
{
    deinit();
}

/**
 * @brief 初始化设备服务
 * @return true 初始化成功
 * @return false 初始化失败
 * @details 创建MQTT客户端并设置消息回调
 */
bool DeviceService::init()
{
    m_mqtt = std::make_unique<MqttService>();

    m_mqtt->setMessageCallback([this](const std::string& topic, const std::string& payload) {
        handleMqttMessage(topic, payload);
    });

    // 初始化红外设备单例（如果AppBridge已经打开，则跳过）
    IRDevice& irDevice = IRDevice::getInstance();
    if (!irDevice.isOpen())
    {
        if (irDevice.open())
        {
            std::cout << "[DeviceService] IR device opened" << std::endl;
        }
        else
        {
            std::cerr << "[DeviceService] Warning: Failed to open IR device" << std::endl;
        }
    }
    else
    {
        std::cout << "[DeviceService] IR device already open (by AppBridge)" << std::endl;
    }

    if (!IRCommandManager::getInstance().init("/data/ir_commands"))
    {
        if (!IRCommandManager::getInstance().init("/root/ir_commands"))
        {
            std::cerr << "[DeviceService] Warning: IR command manager init failed, MQTT IR control unavailable" << std::endl;
        }
        else
        {
            std::cout << "[DeviceService] IR command store: /root/ir_commands" << std::endl;
        }
    }
    else
    {
        std::cout << "[DeviceService] IR command store: /data/ir_commands" << std::endl;
    }

    m_initialized = true;
    std::cout << "[DeviceService] Initialized" << std::endl;
    return true;
}

/**
 * @brief 反初始化设备服务
 * @details 停止传感器上报线程,清空所有设备,MQTT断开连接
 */
void DeviceService::deinit()
{
    std::cout << "[DeviceService] deinit() starting..." << std::endl;

    m_reportRunning = false;
    if (m_reportThread.joinable())
    {
        std::cout << "[DeviceService] Joining report thread..." << std::endl;
        m_reportThread.join();
        std::cout << "[DeviceService] Report thread joined" << std::endl;
    }

    m_leds.clear();
    m_buzzers.clear();
    m_dht11s.clear();
    m_deviceInfos.clear();
    m_sensorDatas.clear();

    std::cout << "[DeviceService] Deinit IRCommandManager..." << std::endl;
    IRCommandManager::getInstance().deinit();

    std::cout << "[DeviceService] Closing IR device..." << std::endl;
    IRDevice::getInstance().close();
    std::cout << "[DeviceService] IR device closed" << std::endl;

    std::cout << "[DeviceService] Resetting MQTT..." << std::endl;
    m_mqtt.reset();
    std::cout << "[DeviceService] MQTT reset done" << std::endl;

    m_initialized = false;
    std::cout << "[DeviceService] Deinitialized" << std::endl;
}

/**
 * @brief 添加LED设备
 * @param deviceId 设备唯一标识符
 * @param gpioPath GPIO设备路径
 * @return true 添加成功
 * @return false 添加失败(设备已存在或GPIO打开失败)
 */
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

/**
 * @brief 打开设备(LED)
 * @param deviceId 设备唯一标识符
 * @return true 操作成功
 * @return false 操作失败(设备不存在或GPIO写入失败)
 * @details 打开LED并发布状态到MQTT
 */
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

/**
 * @brief 关闭设备(LED)
 * @param deviceId 设备唯一标识符
 * @return true 操作成功
 * @return false 操作失败(设备不存在或GPIO写入失败)
 * @details 关闭LED并发布状态到MQTT
 */
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

/**
 * @brief 切换设备状态
 * @param deviceId 设备唯一标识符
 * @return true 操作成功
 * @return false 操作失败
 */
bool DeviceService::toggleDevice(const std::string& deviceId)
{
    bool currentState = getDeviceState(deviceId);
    return currentState ? setDeviceOff(deviceId) : setDeviceOn(deviceId);
}

/**
 * @brief 获取设备状态
 * @param deviceId 设备唯一标识符
 * @return true 设备处于打开状态
 * @return false 设备处于关闭状态或不存在
 */
bool DeviceService::getDeviceState(const std::string& deviceId) const
{
    auto it = m_deviceInfos.find(deviceId);
    if (it == m_deviceInfos.end())
    {
        return false;
    }
    return it->second.state;
}

/**
 * @brief 检查设备是否存在
 * @param deviceId 设备唯一标识符
 * @return true 设备存在
 * @return false 设备不存在
 */
bool DeviceService::hasDevice(const std::string& deviceId) const
{
    return m_deviceInfos.find(deviceId) != m_deviceInfos.end();
}

/**
 * @brief 添加蜂鸣器设备
 * @param deviceId 设备唯一标识符
 * @param gpioPath GPIO设备路径
 * @return true 添加成功
 * @return false 添加失败(设备已存在或GPIO打开失败)
 */
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

/**
 * @brief 打开蜂鸣器
 * @param deviceId 设备唯一标识符
 * @return true 操作成功
 * @return false 操作失败(设备不存在)
 */
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

/**
 * @brief 关闭蜂鸣器
 * @param deviceId 设备唯一标识符
 * @return true 操作成功
 * @return false 操作失败(设备不存在)
 */
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

/**
 * @brief 蜂鸣器响一声
 * @param deviceId 设备唯一标识符
 * @param durationMs 响铃持续时间(毫秒)
 * @return true 操作成功
 * @return false 操作失败(设备不存在)
 */
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

/**
 * @brief 蜂鸣器响多声
 * @param deviceId 设备唯一标识符
 * @param onMs 每声持续时间(毫秒)
 * @param offMs 每声间隔时间(毫秒)
 * @param count 响声次数
 * @return true 操作成功
 * @return false 操作失败(设备不存在)
 */
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

/**
 * @brief 添加DHT11温湿度传感器
 * @param deviceId 设备唯一标识符
 * @param devPath 设备文件路径
 * @return true 添加成功
 * @return false 添加失败(设备已存在或打开失败)
 */
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

/**
 * @brief 获取传感器数据
 * @param deviceId 设备唯一标识符
 * @return SensorData 传感器数据结构(温度、湿度、时间戳)
 * @details 返回缓存的传感器数据,不触发读取
 */
SensorData DeviceService::getSensorData(const std::string& deviceId) const
{
    auto it = m_sensorDatas.find(deviceId);
    if (it == m_sensorDatas.end())
    {
        return SensorData{0, 0, 0};
    }
    return it->second;
}

/**
 * @brief 读取温度
 * @param deviceId 设备唯一标识符
 * @return int 温度值(摄氏度),失败返回0
 * @details 从DHT11读取温度并更新缓存
 */
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

/**
 * @brief 读取湿度
 * @param deviceId 设备唯一标识符
 * @return int 湿度值(%),失败返回0
 * @details 从DHT11读取湿度并更新缓存
 */
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

/**
 * @brief 启用/禁用传感器数据上报
 * @param enable true启用,false禁用
 * @param intervalSec 上报间隔(秒)
 * @details 启动后台线程定期上报传感器数据到MQTT
 */
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

/**
 * @brief 设置传感器上报主题
 * @param topic MQTT主题
 */
void DeviceService::setSensorReportTopic(const std::string& topic)
{
    m_sensorTopic = topic;
}

/**
 * @brief 上报传感器数据
 * @details 读取所有DHT11传感器数据并发布到MQTT
 */
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
                 "{\"temperature\":%d,\"humidity\":%d,\"timestamp\":%ld}",
                 temp, humi, std::time(nullptr));

        std::string topic = m_sensorTopic + "/dht11";
        m_mqtt->publish(topic, payload);

        std::cout << "[DeviceService] Reported sensor data: " << deviceId
                  << " temp=" << temp << " humi=" << humi << std::endl;
    }
}

/**
 * @brief 传感器上报线程
 * @details 后台线程,按设定间隔定期上报传感器数据
 */
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

/**
 * @brief 移除设备
 * @param deviceId 设备唯一标识符
 * @return true 移除成功
 * @return false 设备不存在
 */
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

/**
 * @brief 获取所有设备信息
 * @return std::map<std::string, DeviceInfo> 设备信息映射表
 */
std::map<std::string, DeviceInfo> DeviceService::getAllDevices() const
{
    return m_deviceInfos;
}

/**
 * @brief 设置设备状态变化回调
 * @param callback 回调函数
 */
void DeviceService::setDeviceStateCallback(DeviceStateCallback callback)
{
    m_stateCallback = std::move(callback);
}

/**
 * @brief 设置传感器数据变化回调
 * @param callback 回调函数
 */
void DeviceService::setSensorDataCallback(SensorDataCallback callback)
{
    m_sensorCallback = std::move(callback);
}

/**
 * @brief 连接MQTT服务器
 * @param host 服务器地址
 * @param port 端口号
 * @param clientId 客户端ID
 * @return true 连接成功
 * @return false 连接失败
 */
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

/**
 * @brief 断开MQTT连接
 */
void DeviceService::disconnectMqtt()
{
    if (m_mqtt)
    {
        m_mqtt->disconnect();
    }
}

/**
 * @brief 检查MQTT连接状态
 * @return true 已连接
 * @return false 未连接
 */
bool DeviceService::isMqttConnected() const
{
    return m_mqtt && m_mqtt->isConnected();
}

/**
 * @brief 设置MQTT命令主题
 * @param topic 主题名称
 */
void DeviceService::setMqttCommandTopic(const std::string& topic)
{
    m_commandTopic = topic;
}

/**
 * @brief 设置MQTT状态主题
 * @param topic 主题名称
 */
void DeviceService::setMqttStatusTopic(const std::string& topic)
{
    m_statusTopic = topic;
}

/**
 * @brief 处理MQTT消息
 * @param topic 消息主题
 * @param payload 消息内容
 * @details 解析并执行远程控制命令
 */
void DeviceService::handleMqttMessage(const std::string& topic, const std::string& payload)
{
    std::cout << "[DeviceService] MQTT [" << topic << "]: " << payload << std::endl;

    // LED 控制: rk3506/cmd/led01
    if (topic == "rk3506/cmd/led01")
    {
        if (payload == "ON")
        {
            setDeviceOn("led1");
        }
        else if (payload == "OFF")
        {
            setDeviceOff("led1");
        }
        return;
    }

    // 蜂鸣器控制: rk3506/cmd/buzzer01
    if (topic == "rk3506/cmd/buzzer01")
    {
        if (payload == "ON")
        {
            buzzerOn("buzzer1");
        }
        else if (payload == "OFF")
        {
            buzzerOff("buzzer1");
        }
        else if (payload == "BEEP")
        {
            buzzerBeep("buzzer1", 200);
        }
        else if (payload == "ALARM")
        {
            buzzerBeepPattern("buzzer1", 100, 100, 3);
        }
        return;
    }

    // 传感器控制: rk3506/cmd/sensor
    if (topic == "rk3506/cmd/sensor")
    {
        if (payload == "DHT11_REPORT")
        {
            reportSensorData();
        }
        return;
    }

    // 红外控制: rk3506/cmd/ir
    if (topic == "rk3506/cmd/ir")
    {
        handleIrCommand(payload);
        return;
    }
}

void DeviceService::handleIrCommand(const std::string& payload)
{
    std::cout << "[IR] handleIrCommand: " << payload << std::endl;

    if (payload.empty())
    {
        std::cout << "[IR] Error: empty payload" << std::endl;
        publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", "", "", false, "empty payload");
        return;
    }

    if (payload.front() != '{')
    {
        std::cout << "[IR] Error: not json" << std::endl;
        publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", "", "", false, "invalid json");
        return;
    }

    std::string deviceName;
    std::string cmdName;

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
    }
    catch (const std::exception& e)
    {
        std::cout << "[IR] JSON parse error: " << e.what() << std::endl;
        publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", deviceName, cmdName, false, "json parse error");
        return;
    }

    std::cout << "[IR] Parsed: device=" << deviceName << ", cmd=" << cmdName << std::endl;

    if (deviceName.empty() || cmdName.empty())
    {
        std::cout << "[IR] Error: missing device or cmd" << std::endl;
        publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", deviceName, cmdName, false, "missing device or cmd");
        return;
    }

    std::string mappedDeviceName = mapIrDevice(deviceName);
    std::string irCommandName = resolveIrCommandFromStore(mappedDeviceName, cmdName);

    std::cout << "[IR] Mapped: device=" << mappedDeviceName << ", cmd=" << irCommandName << std::endl;

    if (irCommandName.empty())
    {
        std::cout << "[IR] Error: unknown cmd" << std::endl;
        publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", deviceName, cmdName, false, "unknown cmd");
        return;
    }

    std::cout << "[IR] Emitting: " << mappedDeviceName << " -> " << irCommandName << std::endl;
    bool ok = IRCommandManager::getInstance().emitCommand(mappedDeviceName, irCommandName);
    std::cout << "[IR] Emit result: " << (ok ? "success" : "failed") << std::endl;
    publishIrAck(m_mqtt.get(), "rk3506/cmd/ir", "", deviceName, cmdName, ok, ok ? "success" : "emit failed");
}

std::string DeviceService::mapIrDevice(const std::string& device)
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

std::string DeviceService::mapIrCommand(const std::string& cmd)
{
    static const std::unordered_map<std::string, std::string> cmdMap = {
        {"POWER_ON", "开"},
        {"POWER_OFF", "关"},
        {"TEMP_UP", "升温"},
        {"TEMP_DOWN", "降温"},
        {"MODE_COOL", "制冷"},
        {"MODE_HEAT", "制热"},
    };

    auto it = cmdMap.find(cmd);
    if (it != cmdMap.end())
    {
        return it->second;
    }
    return "";
}

/**
 * @brief 发布设备状态到MQTT
 * @param deviceId 设备唯一标识符
 * @param state 设备状态(true=on, false=off)
 */
void DeviceService::publishDeviceStatus(const std::string& deviceId, bool state)
{
    if (!isMqttConnected())
    {
        return;
    }

    std::string statusTopic = m_statusTopic + "/" + deviceId;
    std::string payload = state ? "ON" : "OFF";

    m_mqtt->publish(statusTopic, payload);
}

/**
 * @brief 订阅MQTT主题
 * @details 连接成功后订阅命令主题
 */
void DeviceService::subscribeMqttTopics()
{
    if (m_mqtt && m_mqtt->isConnected())
    {
        m_mqtt->subscribe("rk3506/cmd/led01");
        m_mqtt->subscribe("rk3506/cmd/buzzer01");
        m_mqtt->subscribe("rk3506/cmd/sensor");
        m_mqtt->subscribe("rk3506/cmd/ir");
        std::cout << "[DeviceService] Subscribed to rk3506/cmd/* topics" << std::endl;
    }
}
