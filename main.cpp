#include "main.h"
#include <csignal>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <lvgl/lv_conf.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/extra/libs/fsdrv/lv_fsdrv.h>
#include "lv_port_init.h"

#include "ui.h"

#include "AppConfig.h"
#include "DeviceService.h"
#include "IoTService.h"
#include "FontManager.h"

static int quit = 0;

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

int main(int argc, char **argv)
{
    signal(2, sigterm_handler);

    lv_port_init();
    lv_fs_posix_init();

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

    /******************************结束******************************************/
    while (quit == 0)
    {
        ui_tick(); // UI刷新
        lv_task_handler(); // LVGL v8 用这个
        usleep(1000); // 1ms，EEZ Studio 推荐的延时
    }

    services_deinit();
    // font_manager_deinit();
    return 0;
}
