#include "main.h"
#include <csignal>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <lvgl/lv_conf.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/extra/libs/fsdrv/lv_fsdrv.h>
#include "lv_port_init.h"

#include "./ui/generated/events_init.h"
#include "./ui/generated/gui_guider.h"
#include "custom.h"

#include "AppBridge.h"
#include "AppConfig.h"
#include "DeviceService.h"
#include "FontManager.h"

lv_ui guider_ui;
static int quit = 0;

static void sigterm_handler(int sig)
{
    std::cerr << "signal " << sig << '\n';
    quit = 1;
}

static int background_services_init(void)
{
    std::cout << "[Main] Initializing background services..." << std::endl;

    auto& deviceService = DeviceService::instance();
    if (!deviceService.init())
    {
        std::cerr << "[Main] Device service init failed" << std::endl;
        return -1;
    }

    deviceService.addLed("led1", APP_GPIO_LED1);
    deviceService.addLed("led2", APP_GPIO_LED2);

    deviceService.addBuzzer("buzzer1", APP_GPIO_BUZZER);

    deviceService.addDht11("dht11_1", APP_DEV_DHT11);

    if (deviceService.connectMqtt(APP_MQTT_HOST, APP_MQTT_PORT, APP_MQTT_CLIENT_ID))
    {
        std::cout << "[Main] MQTT connected to " << APP_MQTT_HOST << std::endl;

        deviceService.enableSensorReport(true, APP_SENSOR_REPORT_INTERVAL);
        std::cout << "[Main] Sensor report enabled, interval: " << APP_SENSOR_REPORT_INTERVAL << "s" << std::endl;
    }
    else
    {
        std::cerr << "[Main] MQTT connection failed, remote control disabled" << std::endl;
    }

    std::cout << "[Main] Background services initialized" << std::endl;
    return 0;
}

static void background_services_deinit(void)
{
    std::cout << "[Main] Deinitializing background services..." << std::endl;

    auto& deviceService = DeviceService::instance();
    deviceService.enableSensorReport(false, 0);
    deviceService.disconnectMqtt();
    deviceService.deinit();

    std::cout << "[Main] Background services deinitialized" << std::endl;
}

int main(int argc, char **argv)
{
    signal(2, sigterm_handler);

    lv_port_init();

    lv_fs_posix_init();

    /*****************************用户程序开始*************************************/

    if (bridge_init() != 0)
    {
        std::cerr << "[Main] Bridge init failed" << '\n';
    }

    if (!font_manager_init())
    {
        std::cerr << "[Main] Font manager init failed" << '\n';
        return -1;
    }

    if (background_services_init() != 0)
    {
        std::cerr << "[Main] Background services init failed" << std::endl;
    }

    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);

    /******************************结束******************************************/
    while (quit == 0)
    {
        lv_task_handler();
        usleep(100);
    }

    background_services_deinit();
    font_manager_deinit();
    bridge_deinit();
    return 0;
}
