/**
 * @file FontManager.h
 * @brief FreeType 字体管理器
 * @details 负责加载和管理应用程序中使用的所有 FreeType 字体样式
 */

#ifndef LVGL_APP_FONT_MANAGER_H
#define LVGL_APP_FONT_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <lvgl/lvgl.h>
#include <stdbool.h>

    /**
     * @brief 初始化所有字体样式
     * @details 加载预定义的中文字体和天气图标字体
     * @return true 成功，false 失败
     */
    bool font_manager_init(void);

    /**
     * @brief 清理字体资源
     */
    void font_manager_deinit(void);

    // ============ 全局字体样式（外部可访问）============

    // 中文字体样式（不同尺寸）
    extern lv_style_t font_style_16;
    extern lv_style_t font_style_24;
    extern lv_style_t font_style_32;
    extern lv_style_t font_style_48;
    extern lv_style_t font_style_64;

    // 天气图标字体样式
    extern lv_style_t weather_font_style_32;
    extern lv_style_t weather_font_style_128;

#ifdef __cplusplus
}
#endif

#endif // LVGL_APP_FONT_MANAGER_H
