#include "main.h"
#include <lvgl/lv_conf.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/extra/libs/fsdrv/lv_fsdrv.h>

#include "./ui/generated/events_init.h"
#include "./ui/generated/gui_guider.h"

#include "AppBridge.h"
#include "FontManager.h"
#include "custom.h"

#include <iostream>

lv_ui guider_ui;
static int quit = 0;

static void sigterm_handler(int sig)
{
    fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

// 定时器回调：用于周期性刷新 UI 数据
void ui_update_timer_cb(lv_timer_t *timer)
{
    // 1. 获取数据 (通过 Bridge)
    float temp = bridge_get_temp();
    float humi = bridge_get_humi();

    // 2. 更新 UI (这里是安全的，因为是在 LVGL 线程内)
    // 示例：假设 guider 生成了 screen_home_label_temp 对象
    // if (guider_ui.screen_home_label_temp) {
    //     lv_label_set_text_fmt(guider_ui.screen_home_label_temp, "Temp: %.1f C", temp);
    // }

    // 打印日志测试
    // LV_LOG_USER("Temp: %.1f, Humi: %.1f", temp, humi);
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigterm_handler);

    // 一切LVGL应用的开始，必须加上这个初始化
    lv_port_init();

    // 初始化 POSIX 文件系统驱动
    lv_fs_posix_init();

    /*****************************用户程序开始*************************************/

    // 初始化 Bridge 层（硬件设备、服务等）
    if (bridge_init() != 0)
    {
        std::cerr << "[Main] Bridge init failed" << std::endl;
    }

    // 初始化字体管理器
    if (!font_manager_init())
    {
        std::cerr << "[Main] Font manager init failed" << std::endl;
        return -1;
    }

    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);

    // 创建一个 LVGL 定时器，每 1000ms 更新一次 UI 数据
    lv_timer_create(ui_update_timer_cb, 1000, NULL);

    /******************************结束******************************************/
    while (!quit)
    {
        /* 调用LVGL任务处理函数，LVGL所有的事件、绘制、送显等都在该接口内完成 */
        lv_task_handler();
        usleep(100);
    }

    font_manager_deinit();
    bridge_deinit();
    return 0;
}
