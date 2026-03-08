#include "main.h"
#include <csignal>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

#include <lvgl/lv_conf.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/extra/libs/fsdrv/lv_fsdrv.h>
#include "lv_port_init.h"

#include "ui.h"

#include "AppConfig.h"
#include "DeviceService.h"
#include "IoTService.h"
#include "FontManager.h"
#include "WeatherService.h"

static int quit = 0;

// 绑定线程到指定 CPU 核心
int bind_thread_to_cpu(pthread_t thread, int cpu_id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    int ret = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
    if (ret != 0)
    {
        fprintf(stderr, "[CPU] pthread_setaffinity_np failed: %s\n", strerror(ret));
        return -1;
    }

    // 验证绑定结果
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(thread, sizeof(cpuset), &cpuset);
    printf("[CPU] Thread bound to CPU %d (actual: ", cpu_id);
    for (int i = 0; i < 3; i++)
    {
        if (CPU_ISSET(i, &cpuset))
            printf("%d ", i);
    }
    printf(")\n");

    return 0;
}

// 设置线程调度策略和优先级
int set_thread_priority(pthread_t thread, int policy, int priority)
{
    struct sched_param param;
    param.sched_priority = priority;

    int ret = pthread_setschedparam(thread, policy, &param);
    if (ret != 0)
    {
        fprintf(stderr, "[CPU] pthread_setschedparam failed: %s\n", strerror(ret));
        return -1;
    }

    const char* policy_str = (policy == SCHED_FIFO) ? "SCHED_FIFO" : (policy == SCHED_RR) ? "SCHED_RR" : "SCHED_OTHER";
    printf("[CPU] Thread priority set: %s, prio=%d\n", policy_str, priority);
    return 0;
}

// 传感器数据采集任务 - 运行在 CPU1
static void* sensor_worker_thread(void* arg)
{
    (void) arg;
    bind_thread_to_cpu(pthread_self(), 1);
    printf("[Worker-1] Sensor worker thread started on CPU1\n");

    auto& deviceService = DeviceService::instance();
    int readInterval = 2000; // 2秒读取一次传感器

    while (quit == 0)
    {
        // 读取 DHT11 传感器数据
        if (deviceService.hasDevice("dht11_1"))
        {
            int temp = deviceService.getTemperature("dht11_1");
            int humi = deviceService.getHumidity("dht11_1");

            if (temp != 0 || humi != 0)
            {
                // 数据成功读取，可以在这里更新缓存或触发回调
                // 实际的上报由 IoTService 的 sensorReportThread 处理
            }
        }

        usleep(readInterval * 1000); // 2ms
    }

    printf("[Worker-1] Sensor worker thread exiting\n");
    return nullptr;
}

// MQTT 和天气数据网络任务 - 运行在 CPU2
static void* network_worker_thread(void* arg)
{
    (void) arg;
    bind_thread_to_cpu(pthread_self(), 2);
    printf("[Worker-2] Network worker thread started on CPU2\n");

    auto& iotService = IoTService::instance();
    auto& weatherService = WeatherService::getInstance();

    // 初始化天气服务
    weatherService.init("衡阳常宁");
    printf("[Worker-2] Weather service initialized\n");

    int weatherUpdateCounter = 0;
    const int WEATHER_UPDATE_INTERVAL = 300; // 每300个循环周期更新一次天气（约5分钟）

    while (quit == 0)
    {
        // 检查 MQTT 连接状态，必要时重连
        if (!iotService.isMqttConnected())
        {
            // 可以尝试重新连接
            // iotService.connectMqtt(APP_MQTT_HOST, APP_MQTT_PORT, APP_MQTT_CLIENT_ID);
        }

        // 定期更新天气数据（在后台线程执行，避免阻塞UI）
        weatherUpdateCounter++;
        if (weatherUpdateCounter >= WEATHER_UPDATE_INTERVAL)
        {
            weatherUpdateCounter = 0;
            printf("[Worker-2] Updating weather data...\n");
            if (weatherService.update())
            {
                printf("[Worker-2] Weather data updated successfully\n");
            }
            else
            {
                printf("[Worker-2] Weather data update failed\n");
            }
        }

        usleep(1000000); // 1秒
    }

    printf("[Worker-2] Network worker thread exiting\n");
    return nullptr;
}

static void sigterm_handler(int sig)
{
    std::cerr << "signal " << sig << '\n';
    quit = 1;
}

static int services_init(void)
{
    std::cout << "[Main] Initializing services..." << std::endl;

    // 初始化硬件设备服务
    auto& deviceService = DeviceService::instance();
    if (!deviceService.init())
    {
        std::cerr << "[Main] Device service init failed" << std::endl;
        return -1;
    }

    // 添加硬件设备
    deviceService.addLed("led1", APP_GPIO_LED1);
    deviceService.addLed("led2", APP_GPIO_LED2);
    deviceService.addBuzzer("buzzer1", APP_GPIO_BUZZER);
    deviceService.addDht11("dht11_1", APP_DEV_DHT11);

    // 初始化 IoT 服务
    auto& iotService = IoTService::instance();
    if (!iotService.init())
    {
        std::cerr << "[Main] IoT service init failed" << std::endl;
        return -1;
    }

    // 初始化 IR 命令管理器
    iotService.initIrCommandManager("/data/ir_commands");

    // 连接 MQTT
    if (iotService.connectMqtt(APP_MQTT_HOST, APP_MQTT_PORT, APP_MQTT_CLIENT_ID))
    {
        std::cout << "[Main] MQTT connected to " << APP_MQTT_HOST << std::endl;

        // 绑定 MQTT 线程到 CPU2
        iotService.bindMqttThreadsToCpu(2);

        // 启用传感器数据上报
        iotService.enableSensorReport(true, APP_SENSOR_REPORT_INTERVAL);
        std::cout << "[Main] Sensor report enabled, interval: " << APP_SENSOR_REPORT_INTERVAL << "s" << std::endl;
    }
    else
    {
        std::cerr << "[Main] MQTT connection failed, remote control disabled" << std::endl;
    }

    std::cout << "[Main] Services initialized" << std::endl;
    return 0;
}

static void services_deinit(void)
{
    std::cout << "[Main] Deinitializing services..." << std::endl;

    auto& iotService = IoTService::instance();
    iotService.disableSensorReport();
    iotService.disconnectMqtt();
    iotService.deinit();

    auto& deviceService = DeviceService::instance();
    deviceService.deinit();

    std::cout << "[Main] Services deinitialized" << std::endl;
}

int main(int argc, char** argv)
{
    signal(2, sigterm_handler);

    // 禁用 stdout 缓冲，确保日志立即输出
    setbuf(stdout, NULL);

    printf("\n========================================\n");
    printf("[Main] LVGL Application Starting...\n");

    // ========== CPU 亲和性设置 ==========
    // 主线程（UI）绑定到 CPU0
    printf("[Main] Binding UI thread to CPU0...\n");
    bind_thread_to_cpu(pthread_self(), 0);
    printf("[Main] LVGL UI thread bound to CPU0\n");

    printf("[Main] Initializing LVGL port...\n");
    lv_port_init();
    lv_fs_posix_init();
    printf("[Main] LVGL port initialized\n");

    /*****************************用户程序开始*************************************/

    // if (!font_manager_init())
    // {
    //     std::cerr << "[Main] Font manager init failed" << std::endl;
    //     return -1;
    // }

    if (services_init() != 0)
    {
        std::cerr << "[Main] Services init failed" << std::endl;
        return -1;
    }

    ui_init(); // 初始化UI

    // 创建后台工作线程 - 分别运行在 CPU1 和 CPU2
    pthread_t sensor_thread, network_thread;
    pthread_create(&sensor_thread, nullptr, sensor_worker_thread, nullptr);
    pthread_create(&network_thread, nullptr, network_worker_thread, nullptr);

    /******************************LVGL 主循环***********************************/
    while (quit == 0)
    {
        ui_tick();         // UI刷新
        lv_task_handler(); // LVGL v8 用这个
        usleep(1000);      // 1ms，EEZ Studio 推荐的延时
    }

    // 等待工作线程结束
    pthread_join(sensor_thread, nullptr);
    pthread_join(network_thread, nullptr);

    services_deinit();
    // font_manager_deinit();
    return 0;
}
