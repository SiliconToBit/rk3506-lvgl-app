/**
 * @file MqttService.h
 * @brief MQTT服务管理类
 * @details 负责MQTT消息的发布和订阅，支持异步回调
 */

#ifndef LVGL_APP_SERVICE_MQTT_SERVICE_H
#define LVGL_APP_SERVICE_MQTT_SERVICE_H

#include "mqtt/async_client.h"
#include <functional>
#include <memory>
#include <string>

class MqttService
{
public:
    using MessageCallback = std::function<void(const std::string& topic, const std::string& payload)>;

    MqttService();
    ~MqttService();

    MqttService(const MqttService&) = delete;
    MqttService& operator=(const MqttService&) = delete;

    bool connect(const std::string& host, int port, const std::string& clientId = "lvgl_client");
    void disconnect();
    bool isConnected() const;

    bool subscribe(const std::string& topic, int qos = 1);
    bool unsubscribe(const std::string& topic);

    bool publish(const std::string& topic, const std::string& payload, int qos = 1, bool retained = false);
    bool publish(const std::string& topic, const void* data, size_t size, int qos = 1, bool retained = false);

    void setMessageCallback(MessageCallback callback);

    void setReconnectInterval(int seconds);
    void enableAutoReconnect(bool enable);

private:
    class MqttCallback;
    friend class MqttCallback;

    bool tryReconnect();

    std::unique_ptr<mqtt::async_client> m_client;
    std::unique_ptr<mqtt::connect_options> m_connOpts;
    std::unique_ptr<MqttCallback> m_callback;
    MessageCallback m_messageCallback;
    std::string m_serverAddress;
    bool m_connected;
    bool m_autoReconnect;
    int m_reconnectInterval;
};

#endif
