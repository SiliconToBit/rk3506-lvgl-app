/**
 * @file FontManager.c
 * @brief FreeType 字体管理器实现
 */

#include "FontManager.h"
#include "AppConfig.h"
#include <lvgl/src/extra/libs/freetype/lv_freetype.h>
#include <stdio.h>

// ============ 全局字体样式定义 ============
lv_style_t font_style_16;
lv_style_t font_style_24;
lv_style_t font_style_32;
lv_style_t font_style_48;
lv_style_t font_style_64;

lv_style_t weather_font_style_32;
lv_style_t weather_font_style_128;

// 字体信息存储（最多支持10个字体）
#define MAX_FONTS 10
static lv_ft_info_t ft_info[MAX_FONTS];
static uint8_t font_count = 0;

/**
 * @brief 创建指定大小的 FreeType 字体并关联到样式
 * @param font_path 字体文件路径
 * @param size 字体大小（像素）
 * @param style 要应用字体的样式指针
 * @return true 成功，false 失败
 */
static bool create_freetype_font(const char *font_path, uint16_t size, lv_style_t *style)
{
    if (font_count >= MAX_FONTS)
    {
        LV_LOG_ERROR("Too many fonts, max is %d", MAX_FONTS);
        return false;
    }

    // 配置字体信息
    ft_info[font_count].name = font_path;
    ft_info[font_count].weight = size;
    ft_info[font_count].style = FT_FONT_STYLE_NORMAL;
    ft_info[font_count].mem = NULL;

    // 初始化字体
    if (!lv_ft_font_init(&ft_info[font_count]))
    {
        LV_LOG_ERROR("Failed to create font: %s (size %d)", font_path, size);
        return false;
    }

    // 设置回退字体以支持符号和图标
    if (ft_info[font_count].font)
    {
        // 根据字体大小选择最接近的内置字体作为 fallback
        if (size >= 32)
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_32;
        }
        else if (size >= 22)
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_22;
        }
        else if (size >= 20)
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_20;
        }
        else if (size >= 18)
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_18;
        }
        else if (size >= 16)
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_16;
        }
        else
        {
            ft_info[font_count].font->fallback = &lv_font_montserrat_14;
        }
    }

    // 初始化样式并设置字体
    lv_style_init(style);
    lv_style_set_text_font(style, ft_info[font_count].font);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);

    font_count++;
    LV_LOG_USER("Loaded font: %s (size %d)", font_path, size);
    return true;
}

/**
 * @brief 检查字体文件是否存在
 * @param font_path 字体文件路径
 * @return true 存在，false 不存在
 */
static bool check_font_file(const char *font_path)
{
    FILE *fp = fopen(font_path, "r");
    if (fp == NULL)
    {
        LV_LOG_ERROR("Font file not found: %s", font_path);
        perror("fopen");
        return false;
    }
    fclose(fp);
    return true;
}

bool font_manager_init(void)
{
    LV_LOG_USER("Initializing font manager...");

    // 获取字体路径
    const char *font_path = APP_FONT_PATH;
    const char *weather_font_path = APP_WEATHER_FONT_PATH;

    // 检查字体文件是否存在
    if (!check_font_file(font_path))
    {
        return false;
    }

    if (!check_font_file(weather_font_path))
    {
        return false;
    }

    // 创建中文字体（不同尺寸）
    if (!create_freetype_font(font_path, 16, &font_style_16))
        return false;
    if (!create_freetype_font(font_path, 24, &font_style_24))
        return false;
    if (!create_freetype_font(font_path, 32, &font_style_32))
        return false;
    if (!create_freetype_font(font_path, 48, &font_style_48))
        return false;
    if (!create_freetype_font(font_path, 64, &font_style_64))
        return false;

    // 创建天气图标字体
    if (!create_freetype_font(weather_font_path, 32, &weather_font_style_32))
        return false;
    if (!create_freetype_font(weather_font_path, 128, &weather_font_style_128))
        return false;

    LV_LOG_USER("Font manager initialized successfully (%d fonts loaded)", font_count);
    return true;
}

void font_manager_deinit(void)
{
    // FreeType 字体资源由 LVGL 管理，通常在退出时自动清理
    // 这里可以添加额外的清理逻辑（如果需要）
    LV_LOG_USER("Font manager deinitialized");
}
