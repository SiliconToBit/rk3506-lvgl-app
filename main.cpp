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
#include "FontManager.h"



lv_ui guider_ui;
static int quit = 0;

static void sigterm_handler(int sig)
{
    std::cerr << "signal " << sig << '\n';
    quit = 1;
}



int main(int argc, char **argv)
{
    signal(2, sigterm_handler);

    // 一切LVGL应用的开始，必须加上这个初始化
    lv_port_init();

    // 初始化 POSIX 文件系统驱动
    lv_fs_posix_init();

    /*****************************用户程序开始*************************************/

    // 初始化 Bridge 层（硬件设备、服务等）
    if (bridge_init() != 0)
    {
        std::cerr << "[Main] Bridge init failed" << '\n';
    }

    // 初始化字体管理器
    if (!font_manager_init())
    {
        std::cerr << "[Main] Font manager init failed" << '\n';
        return -1;
    }


    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);


    /******************************结束******************************************/
    while (quit == 0)
    {
        /* 调用LVGL任务处理函数，LVGL所有的事件、绘制、送显等都在该接口内完成 */
        lv_task_handler();
        usleep(100);
    }

    font_manager_deinit();
    bridge_deinit();
    return 0;
}
