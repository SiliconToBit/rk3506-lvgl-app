#include "main.h"
#include "app_bridge.h"
#include <lvgl/lv_conf.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/extra/libs/fsdrv/lv_fsdrv.h>

#include "./ui/generated/events_init.h"
#include "./ui/generated/gui_guider.h"

#include "AudioDevice.h"
#include "app_bridge.h"
#include "custom.h"


lv_ui guider_ui;

static int quit = 0;

// 全局字体样式定义
lv_style_t font_style_16;
lv_style_t font_style_24;
lv_style_t font_style_32;
lv_style_t font_style_48;
lv_style_t font_style_64;

lv_style_t weather_font_style_32;
lv_style_t weather_font_style_128;


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

/**
 * @brief 创建指定大小的 FreeType 字体并关联到样式
 * @param font_path 字体文件路径
 * @param size 字体大小
 * @param style 要应用字体的样式指针
 * @return true 成功，false 失败
 */
static bool create_freetype_font(const char *font_path, uint16_t size, lv_style_t *style)
{
    static lv_ft_info_t ft_info[10]; // 支持最多5种字体大小
    static uint8_t font_index = 0;

    if (font_index >= 10)
    {
        LV_LOG_ERROR("Too many fonts created, max is 10");
        return false;
    }

    ft_info[font_index].name = font_path;
    ft_info[font_index].weight = size;
    ft_info[font_index].style = FT_FONT_STYLE_NORMAL;
    ft_info[font_index].mem = NULL;

    if (!lv_ft_font_init(&ft_info[font_index]))
    {
        LV_LOG_ERROR("Failed to create font size %d", size);
        return false;
    }

    // 设置回退字体以支持图标 (Symbols)
    if (ft_info[font_index].font)
    {
        // 根据字体大小选择最接近的内置字体作为 fallback
        // 注意：需要在 lv_conf.h 中启用对应的内置字体
        if (size >= 32)
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_32;
        }
        else if (size >= 22)
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_22;
        }
        else if (size >= 20)
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_20;
        }
        else if (size >= 18)
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_18;
        }
        else if (size >= 16)
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_16;
        }
        else
        {
            ft_info[font_index].font->fallback = &lv_font_montserrat_14;
        }
    }

    // 初始化样式并设置字体
    lv_style_init(style);
    lv_style_set_text_font(style, ft_info[font_index].font);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);

    font_index++;
    LV_LOG_USER("Created font size %d successfully", size);
    return true;
}

/**
 * @brief 初始化所有预定义的 FreeType 字体样式
 */
static void lv_freetype_fonts_init(void)
{
    // 使用开发板上的字体路径
    const char *font_path = "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf";
    const char *weather_font_path ="/usr/share/fonts/qweather-icons.ttf";

    // 检查字体文件是否存在
    FILE *fp = fopen(font_path, "r");
    if (fp == NULL)
    {
        LV_LOG_ERROR("Font file not found: %s", font_path);
        perror("fopen");
        return;
    }
    fclose(fp);

    FILE *fp_weather = fopen(weather_font_path, "r");
    if (fp_weather == NULL)
    {
        LV_LOG_ERROR("Weather font file not found: %s", weather_font_path);
        perror("fopen");
        return;
    }
    fclose(fp_weather);

    // 创建不同大小的字体样式
    create_freetype_font(font_path, 16, &font_style_16);
    create_freetype_font(font_path, 24, &font_style_24);
    create_freetype_font(font_path, 32, &font_style_32);
    create_freetype_font(font_path, 48, &font_style_48);
    create_freetype_font(font_path, 64, &font_style_64);

    create_freetype_font(weather_font_path, 32, &weather_font_style_32);
    create_freetype_font(weather_font_path, 128, &weather_font_style_128);


    LV_LOG_USER("All FreeType fonts initialized");
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigterm_handler);

    // 一切LVGL应用的开始，必须加上这个初始化
    lv_port_init();

    // 初始化 POSIX 文件系统驱动
    lv_fs_posix_init();


    /*****************************用户程序开始*************************************/

    // bridge_music_play();

    // 初始化所有字体样式
    lv_freetype_fonts_init();
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

    return 0;
}
