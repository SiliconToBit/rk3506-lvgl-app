#include "MqttService.h"
#include "mqtt/async_client.h"
#include <chrono>
#include <iostream>
#include <thread>

class MqttService::MqttCallback : public virtual mqtt::callback
{
public:
    MqttCallback(MqttService* service) : m_service(service) {}

    void connected(const std::string& cause) override
    {
        m_service->m_connected = true;
        std::cout << "[MQTT] Connected" << std::endl;
    }

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

    void message_arrived(mqtt::const_message_ptr msg) override
    {
        if (m_service->m_messageCallback)
        {
            m_service->m_messageCallback(msg->get_topic(), msg->get_payload());
        }
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {}

private:
    MqttService* m_service;
};

MqttService::MqttService()
    : m_connected(false)
    , m_autoReconnect(true)
    , m_reconnectInterval(5)
{
}

MqttService::~MqttService()
{
    disconnect();
}

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

        std::cout << "[MQTT] Connecting to " << m_serverAddress << "..." << std::endl;

        auto token = m_client->connect(*m_connOpts);
        token->wait();

        m_connected = true;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Connect error: " << e.what() << std::endl;
        m_connected = false;
        return false;
    }
}

void MqttService::disconnect()
{
    if (m_client && m_connected)
    {
        try
        {
            auto token = m_client->disconnect();
            token->wait();
            std::cout << "[MQTT] Disconnected" << std::endl;
        }
        catch (const mqtt::exception& e)
        {
            std::cerr << "[MQTT] Disconnect error: " << e.what() << std::endl;
        }
    }
    m_connected = false;
}

bool MqttService::isConnected() const
{
    return m_connected && m_client && m_client->is_connected();
}

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
        token->wait();
        std::cout << "[MQTT] Subscribed to: " << topic << std::endl;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Subscribe error: " << e.what() << std::endl;
        return false;
    }
}

bool MqttService::unsubscribe(const std::string& topic)
{
    if (!isConnected())
    {
        return false;
    }

    try
    {
        auto token = m_client->unsubscribe(topic);
        token->wait();
        std::cout << "[MQTT] Unsubscribed from: " << topic << std::endl;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Unsubscribe error: " << e.what() << std::endl;
        return false;
    }
}

bool MqttService::publish(const std::string& topic, const std::string& payload, int qos, bool retained)
{
    return publish(topic, payload.c_str(), payload.size(), qos, retained);
}

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
        token->wait();
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Publish error: " << e.what() << std::endl;
        return false;
    }
}

void MqttService::setMessageCallback(MessageCallback callback)
{
    m_messageCallback = std::move(callback);
}

void MqttService::setReconnectInterval(int seconds)
{
    m_reconnectInterval = seconds;
}

void MqttService::enableAutoReconnect(bool enable)
{
    m_autoReconnect = enable;
}

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
        token->wait();
        m_connected = true;
        return true;
    }
    catch (const mqtt::exception& e)
    {
        std::cerr << "[MQTT] Reconnect failed: " << e.what() << std::endl;
        return false;
    }
}
