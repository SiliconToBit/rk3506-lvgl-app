/**
 * @file MqttService.h
 * @brief MQTT服务管理类
 * @details 负责MQTT消息的发布和订阅，支持异步回调
 */

#ifndef LVGL_APP_SERVICE_MQTT_SERVICE_H
#define LVGL_APP_SERVICE_MQTT_SERVICE_H

#include "mqtt/async_client.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <pthread.h>
#include <sched.h>
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

    // 绑定 MQTT 线程到指定 CPU
    void bindMqttThreadsToCpu(int cpu_id);

private:
    class MqttCallback;
    friend class MqttCallback;

    void enqueueMessage(std::string topic, std::string payload);
    void startMessageWorker();
    void stopMessageWorker();
    void messageWorkerLoop();

    bool tryReconnect();

    std::unique_ptr<mqtt::async_client> m_client;
    std::unique_ptr<mqtt::connect_options> m_connOpts;
    std::unique_ptr<MqttCallback> m_callback;
    MessageCallback m_messageCallback;
    std::string m_serverAddress;
    bool m_connected;
    bool m_autoReconnect;
    int m_reconnectInterval;

    std::thread m_messageThread;
    std::mutex m_messageMutex;
    std::condition_variable m_messageCv;
    std::queue<std::pair<std::string, std::string>> m_messageQueue;
    bool m_messageRunning;
};

#endif
