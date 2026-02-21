#include "MqttService.h"
#include "mqtt/async_client.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace
{
constexpr std::chrono::seconds kMqttWaitTimeout(3);
}

/**
 * @brief MQTT回调类
 * @details 继承mqtt::callback,处理连接状态变化和消息到达事件
 */
class MqttService::MqttCallback : public virtual mqtt::callback
{
public:
    /**
     * @brief 构造函数
     * @param service MqttService实例指针
     */
    MqttCallback(MqttService* service) : m_service(service) {}

    /**
     * @brief 连接成功回调
     * @param cause 连接原因(通常为空)
     */
    void connected(const std::string& cause) override
    {
        m_service->m_connected = true;
        std::cout << "[MQTT] Connected" << std::endl;
    }

    /**
     * @brief 连接丢失回调
     * @param cause 丢失原因
     * @details 连接丢失时启动自动重连线程
     */
    void connection_lost(const std::string& cause) override
    {
        m_service->m_connected = false;
        std::cout << "[MQTT] Connection lost: " << cause << std::endl;

        if (m_service->m_autoReconnect)
        {
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::seconds(m_service->m_reconnectInterval));
                m_service->tryReconnect();
            }).detach();
        }
    }

    /**
     * @brief 消息到达回调
     * @param msg 消息指针
     * @details 收到消息时调用用户注册的消息回调函数
     */
    void message_arrived(mqtt::const_message_ptr msg) override
    {
        m_service->enqueueMessage(msg->get_topic(), msg->get_payload());
    }

    /**
     * @brief 消息投递完成回调
     * @param token 投递令牌
     */
    void delivery_complete(mqtt::delivery_token_ptr token) override {}

private:
    MqttService* m_service;
};

/**
 * @brief 构造函数
 * @details 初始化连接状态和重连参数
 */
MqttService::MqttService()
    : m_connected(false)
    , m_autoReconnect(true)
    , m_reconnectInterval(5)
    , m_messageRunning(false)
{
}

/**
 * @brief 析构函数
 * @details 自动断开MQTT连接
 */
MqttService::~MqttService()
{
    disconnect();
}

/**
 * @brief 连接MQTT服务器
 * @param host 服务器地址
 * @param port 端口号
 * @param clientId 客户端唯一标识
 * @return true 连接成功
 * @return false 连接失败
 * @details 创建异步客户端,设置连接选项和回调,发起连接
 */
bool MqttService::connect(const std::string& host, int port, const std::string& clientId)
{
    try
    {
        m_serverAddress = "tcp://" + host + ":" + std::to_string(port);

        m_client = std::make_unique<mqtt::async_client>(m_serverAddress, clientId);
        m_connOpts = std::make_unique<mqtt::connect_options>();

        m_connOpts->set_clean_session(true);
        m_connOpts->set_keep_alive_interval(std::chrono::seconds(60));
        m_connOpts->set_automatic_reconnect(std::chrono::seconds(5), std::chrono::seconds(30));

        m_callback = std::make_unique<MqttCallback>(this);
        m_client->set_callback(*m_callback);
        startMessageWorker();

        std::cout << "[MQTT] Connecting to " << m_serverAddress << "..." << std::endl;

        auto token = m_client->connect(*m_connOpts);
        if (!token->wait_for(kMqttWaitTimeout))
        {
            std::cerr << "[MQTT] Connect timeout" << std::endl;
            return false;
        }

        m_connected = m_client->is_connected();
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Connect error: " << e.what() << std::endl;
        m_connected = false;
        return false;
    }
}

/**
 * @brief 断开MQTT连接
 * @details 同步断开连接并更新连接状态
 */
void MqttService::disconnect()
{
    std::cout << "[MQTT] disconnect() called, connected=" << m_connected << std::endl;
    if (m_client && m_connected)
    {
        try
        {
            std::cout << "[MQTT] Calling client->disconnect()..." << std::endl;
            auto token = m_client->disconnect();
            if (token->wait_for(kMqttWaitTimeout))
            {
                std::cout << "[MQTT] Disconnected" << std::endl;
            }
            else
            {
                std::cerr << "[MQTT] Disconnect timeout" << std::endl;
            }
        }
        catch (const mqtt::exception& e)
        {
            std::cerr << "[MQTT] Disconnect error: " << e.what() << std::endl;
        }
    }
    m_connected = false;
    stopMessageWorker();
    std::cout << "[MQTT] disconnect() done" << std::endl;
}

/**
 * @brief 检查连接状态
 * @return true 已连接
 * @return false 未连接
 */
bool MqttService::isConnected() const
{
    return m_connected && m_client && m_client->is_connected();
}

/**
 * @brief 订阅主题
 * @param topic 主题名称
 * @param qos 服务质量等级(默认1)
 * @return true 订阅成功
 * @return false 订阅失败
 */
bool MqttService::subscribe(const std::string& topic, int qos)
{
    if (!isConnected())
    {
        std::cerr << "[MQTT] Not connected, cannot subscribe" << std::endl;
        return false;
    }

    try
    {
        auto token = m_client->subscribe(topic, qos);
        if (!token->wait_for(kMqttWaitTimeout))
        {
            std::cerr << "[MQTT] Subscribe timeout: " << topic << std::endl;
            return false;
        }
        std::cout << "[MQTT] Subscribed to: " << topic << std::endl;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Subscribe error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 取消订阅主题
 * @param topic 主题名称
 * @return true 取消成功
 * @return false 取消失败
 */
bool MqttService::unsubscribe(const std::string& topic)
{
    if (!isConnected())
    {
        return false;
    }

    try
    {
        auto token = m_client->unsubscribe(topic);
        if (!token->wait_for(kMqttWaitTimeout))
        {
            std::cerr << "[MQTT] Unsubscribe timeout: " << topic << std::endl;
            return false;
        }
        std::cout << "[MQTT] Unsubscribed from: " << topic << std::endl;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Unsubscribe error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 发布消息(字符串)
 * @param topic 主题名称
 * @param payload 消息内容
 * @param qos 服务质量等级(默认1)
 * @param retained 是否保留消息(默认false)
 * @return true 发布成功
 * @return false 发布失败
 */
bool MqttService::publish(const std::string& topic, const std::string& payload, int qos, bool retained)
{
    return publish(topic, payload.c_str(), payload.size(), qos, retained);
}

/**
 * @brief 发布消息(二进制数据)
 * @param topic 主题名称
 * @param data 数据指针
 * @param size 数据大小
 * @param qos 服务质量等级(默认1)
 * @param retained 是否保留消息(默认false)
 * @return true 发布成功
 * @return false 发布失败
 */
bool MqttService::publish(const std::string& topic, const void* data, size_t size, int qos, bool retained)
{
    if (!isConnected())
    {
        std::cerr << "[MQTT] Not connected, cannot publish" << std::endl;
        return false;
    }

    try
    {
        mqtt::message_ptr msg = mqtt::make_message(topic, data, size, qos, retained);
        auto token = m_client->publish(msg);
        if (!token->wait_for(kMqttWaitTimeout))
        {
            std::cerr << "[MQTT] Publish timeout: " << topic << std::endl;
            return false;
        }
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Publish error: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief 设置消息回调函数
 * @param callback 回调函数,参数为(主题, 消息内容)
 */
void MqttService::setMessageCallback(MessageCallback callback)
{
    m_messageCallback = std::move(callback);
}

/**
 * @brief 设置重连间隔
 * @param seconds 间隔秒数
 */
void MqttService::setReconnectInterval(int seconds)
{
    m_reconnectInterval = seconds;
}

/**
 * @brief 启用/禁用自动重连
 * @param enable true启用, false禁用
 */
void MqttService::enableAutoReconnect(bool enable)
{
    m_autoReconnect = enable;
}

/**
 * @brief 尝试重新连接
 * @return true 重连成功
 * @return false 重连失败
 */
bool MqttService::tryReconnect()
{
    if (m_connected)
    {
        return true;
    }

    std::cout << "[MQTT] Attempting to reconnect..." << std::endl;

    try
    {
        auto token = m_client->connect(*m_connOpts);
        if (!token->wait_for(kMqttWaitTimeout))
        {
            std::cerr << "[MQTT] Reconnect timeout" << std::endl;
            return false;
        }
        m_connected = m_client->is_connected();
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Reconnect failed: " << e.what() << std::endl;
        return false;
    }
}

void MqttService::enqueueMessage(std::string topic, std::string payload)
{
    std::lock_guard<std::mutex> lock(m_messageMutex);
    if (!m_messageRunning)
    {
        return;
    }

    m_messageQueue.emplace(std::move(topic), std::move(payload));
    m_messageCv.notify_one();
}

void MqttService::startMessageWorker()
{
    stopMessageWorker();

    {
        std::lock_guard<std::mutex> lock(m_messageMutex);
        m_messageRunning = true;
    }

    m_messageThread = std::thread(&MqttService::messageWorkerLoop, this);
}

void MqttService::stopMessageWorker()
{
    {
        std::lock_guard<std::mutex> lock(m_messageMutex);
        m_messageRunning = false;
    }
    m_messageCv.notify_all();

    if (m_messageThread.joinable())
    {
        m_messageThread.join();
    }

    std::lock_guard<std::mutex> lock(m_messageMutex);
    std::queue<std::pair<std::string, std::string>> empty;
    m_messageQueue.swap(empty);
}

void MqttService::messageWorkerLoop()
{
    while (true)
    {
        std::pair<std::string, std::string> message;
        {
            std::unique_lock<std::mutex> lock(m_messageMutex);
            m_messageCv.wait(lock, [this]() {
                return !m_messageRunning || !m_messageQueue.empty();
            });

            if (!m_messageRunning && m_messageQueue.empty())
            {
                break;
            }

            message = std::move(m_messageQueue.front());
            m_messageQueue.pop();
        }

        auto callback = m_messageCallback;
        if (callback)
        {
            callback(message.first, message.second);
        }
    }
}
