// ============================================================================
// EEZ Studio Actions 动作实现
//
// 此文件放在 actions/ 目录下，不会被 EEZ Studio 覆盖
// ============================================================================

#include "../ui/actions.h"
#include "../ui/images.h"
#include "../ui/screens.h"
#include "../ui/fonts.h"
#include "../service/MusicActionService.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>

namespace
{
    // 全局变量
    std::string g_music_cover_path;             // 当前音乐封面路径
    std::string g_last_song_title;              // 上一首歌曲标题（用于检测歌曲切换）
    lv_timer_t* g_music_ui_timer = nullptr;     // 音乐界面UI刷新定时器
    lv_timer_t* g_weather_time_timer = nullptr; // 天气界面时间刷新定时器
    bool g_slider_syncing = false;              // 标记是否正在同步滑块值（防止事件循环）
    std::vector<lv_obj_t*> g_song_buttons;      // 歌曲列表按钮容器
    int g_last_minute = -1;                     // 上次更新的分钟数，用于检测分钟变化
    std::string g_selected_wifi_ssid;           // 当前选中的WiFi SSID

    // 前置声明
    void refresh_music_page_ui();
    void refresh_music_playback_ui();

    // 歌曲按钮点击事件回调
    void song_button_click_cb(lv_event_t* e);

    /**
     * @brief 刷新歌曲列表显示
     * @details 在 scroll_list_area 中动态创建歌曲按钮，每个按钮显示歌曲名
     *          点击按钮可播放对应歌曲
     */
    void refresh_song_list()
    {
        if (!objects.scroll_list_area)
        {
            return;
        }

        // 清除旧的按钮
        for (auto btn : g_song_buttons)
        {
            if (btn)
            {
                lv_obj_del(btn);
            }
        }
        g_song_buttons.clear();

        // 获取播放列表
        MusicActionService& musicService = MusicActionService::getInstance();
        std::vector<std::string> playlist = musicService.getPlaylist();

        if (playlist.empty())
        {
            std::cout << "[EEZ] No songs in playlist" << std::endl;
            return;
        }

        std::cout << "[EEZ] Refreshing song list, count: " << playlist.size() << std::endl;

        // 创建歌曲按钮
        for (size_t i = 0; i < playlist.size(); ++i)
        {
            lv_obj_t* btn = lv_btn_create(objects.scroll_list_area);
            lv_obj_set_size(btn, 200, 40);
            lv_obj_add_event_cb(btn, song_button_click_cb, LV_EVENT_CLICKED, nullptr);
            lv_obj_set_user_data(btn, (void*) (intptr_t) i);

            // 设置按钮样式
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x666666), LV_PART_MAIN | LV_STATE_PRESSED);

            // 创建标签
            lv_obj_t* label = lv_label_create(btn);
            lv_label_set_text(label, playlist[i].c_str());
            lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(label, ui_font_alibaba_pu_hui_ti_24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_center(label);

            g_song_buttons.push_back(btn);
        }
    }

    /**
     * @brief 歌曲按钮点击事件回调
     * @param e LVGL事件对象
     * @details 获取点击的歌曲索引，播放对应歌曲，并更新播放/暂停按钮状态
     */
    void song_button_click_cb(lv_event_t* e)
    {
        lv_obj_t* btn = lv_event_get_target(e);
        int index = (int) (intptr_t) lv_obj_get_user_data(btn);

        std::cout << "[EEZ] Song clicked, index: " << index << std::endl;
        MusicActionService::getInstance().playAtIndex(index);

        // 更新播放/暂停按钮状态为播放中（显示暂停图标）
        if (objects.music_play_pause_imgbtn)
        {
            lv_obj_add_state(objects.music_play_pause_imgbtn, LV_STATE_CHECKED);
            // 强制刷新按钮状态以更新图片
            lv_obj_invalidate(objects.music_play_pause_imgbtn);
        }

        refresh_music_page_ui();
    }

    /**
     * @brief 设置滑块值但不触发事件
     * @param slider 滑块对象
     * @param value 要设置的值
     * @details 使用 g_slider_syncing 标志防止事件循环
     */
    void set_slider_value_no_event(lv_obj_t* slider, int value)
    {
        if (!slider)
        {
            return;
        }
        g_slider_syncing = true;
        lv_slider_set_value(slider, value, LV_ANIM_OFF);
        g_slider_syncing = false;
    }

    /**
     * @brief 格式化时间为 MM:SS 字符串
     * @param sec 秒数
     * @return 格式化后的时间字符串，如 "03:45"
     */
    std::string format_time_label(double sec)
    {
        if (sec < 0)
        {
            sec = 0;
        }

        int totalSeconds = static_cast<int>(sec);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        char buffer[16] = {0};
        std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return std::string(buffer);
    }

    /**
     * @brief 刷新音乐播放UI（时间、进度、歌词等）
     * @details 更新播放时间、总时长、进度条、歌词等动态内容
     *          如果歌曲切换，还会更新标题和封面
     */
    void refresh_music_playback_ui()
    {
        MusicActionService& musicService = MusicActionService::getInstance();
        const std::string title = musicService.getCurrentSongName();
        const double current = musicService.getCurrentTime();
        const double total = musicService.getTotalDuration();
        const std::string currentTime = format_time_label(current);
        const std::string totalTime = format_time_label(total);
        const std::string lyricLine = musicService.getCurrentLyricLine();

        // 歌曲切换时更新标题和封面
        if (title != g_last_song_title)
        {
            g_last_song_title = title;

            if (objects.music_title_label)
            {
                lv_label_set_text(objects.music_title_label, title.empty() ? "暂无歌曲" : title.c_str());
            }

            g_music_cover_path = musicService.getCurrentAlbumCoverPath();
            if (objects.music_album_img)
            {
                if (!g_music_cover_path.empty())
                {
                    lv_img_set_src(objects.music_album_img, g_music_cover_path.c_str());
                }
                else
                {
                    lv_img_set_src(objects.music_album_img, &img_music_image);
                }
            }
        }

        // 更新当前时间
        if (objects.music_curr_time_label)
        {
            lv_label_set_text(objects.music_curr_time_label, currentTime.c_str());
        }

        // 更新总时长
        if (objects.music_total_time_label)
        {
            lv_label_set_text(objects.music_total_time_label, totalTime.c_str());
        }

        // 更新歌词
        if (objects.music_lyrics_label)
        {
            lv_label_set_text(objects.music_lyrics_label, lyricLine.empty() ? "..." : lyricLine.c_str());
        }

        // 更新进度条
        if (objects.music_progress_slider)
        {
            int progress = 0;
            if (total > 0.0)
            {
                progress = static_cast<int>((current / total) * 100.0);
            }
            if (progress < 0)
            {
                progress = 0;
            }
            else if (progress > 100)
            {
                progress = 100;
            }
            set_slider_value_no_event(objects.music_progress_slider, progress);
        }
    }

    /**
     * @brief 刷新音乐页面UI（标题、封面、音量等）
     * @details 更新静态内容（标题、封面）和音量滑块
     */
    void refresh_music_page_ui()
    {
        MusicActionService& musicService = MusicActionService::getInstance();

        const std::string title = musicService.getCurrentSongName();
        g_last_song_title = title;
        g_music_cover_path = musicService.getCurrentAlbumCoverPath();

        // 更新标题
        if (objects.music_title_label)
        {
            lv_label_set_text(objects.music_title_label, title.empty() ? "暂无歌曲" : title.c_str());
        }

        // 更新封面
        if (objects.music_album_img)
        {
            if (!g_music_cover_path.empty())
            {
                lv_img_set_src(objects.music_album_img, g_music_cover_path.c_str());
            }
            else
            {
                lv_img_set_src(objects.music_album_img, &img_music_image);
            }
        }

        // 更新音量滑块
        if (objects.music_volume_slider)
        {
            int volume = static_cast<int>(musicService.getVolume());
            if (volume < 0)
            {
                volume = 0;
            }
            else if (volume > 100)
            {
                volume = 100;
            }
            set_slider_value_no_event(objects.music_volume_slider, volume);
        }

        refresh_music_playback_ui();
    }

    /**
     * @brief 音乐UI定时器回调
     * @param timer LVGL定时器对象
     * @details 每200ms刷新一次播放UI，更新时间和进度
     */
    void music_ui_timer_cb(lv_timer_t* timer)
    {
        LV_UNUSED(timer);
        refresh_music_playback_ui();
    }
} // namespace

// ============================================================================
// 音乐相关动作
// ============================================================================

/**
 * @brief 初始化音乐界面
 * @param e LVGL事件对象
 * @details 初始化音乐服务，设置默认音量，启动UI刷新定时器
 */
extern "C" void action_init_music_screen(lv_event_t* e)
{
    std::cout << "[EEZ] Init music screen" << std::endl;
    MusicActionService& musicService = MusicActionService::getInstance();
    musicService.initScreen();
    musicService.setVolume(75); // 默认音量 75%，对应 DAC VOLUME 180
    refresh_music_page_ui();

    if (!g_music_ui_timer)
    {
        g_music_ui_timer = lv_timer_create(music_ui_timer_cb, 200, nullptr);
    }
}

/**
 * @brief 反初始化音乐界面
 * @param e LVGL事件对象
 * @details 停止音乐服务，删除定时器，清理歌曲列表
 */
extern "C" void action_deinit_music_screen(lv_event_t* e)
{
    std::cout << "[EEZ] Deinit music screen" << std::endl;
    MusicActionService::getInstance().deinitScreen();
    if (g_music_ui_timer)
    {
        lv_timer_del(g_music_ui_timer);
        g_music_ui_timer = nullptr;
    }
    g_last_song_title.clear();

    // 清理歌曲列表按钮
    for (auto btn : g_song_buttons)
    {
        if (btn)
        {
            lv_obj_del(btn);
        }
    }
    g_song_buttons.clear();
}

/**
 * @brief 加载音乐目录
 * @param e LVGL事件对象
 * @details 扫描音乐目录，加载播放列表，显示歌曲列表
 */
extern "C" void action_load_music_directory(lv_event_t* e)
{
    std::cout << "[EEZ] Loading music directory..." << std::endl;
    MusicActionService::getInstance().loadDirectory();
    // 加载目录后刷新歌曲列表显示
    refresh_song_list();
    refresh_music_page_ui();
}

/**
 * @brief 切换播放/暂停状态
 * @param e LVGL事件对象
 * @details 根据按钮的checked状态决定播放或暂停
 *          checked=true 表示当前是暂停图标，点击后播放
 *          checked=false 表示当前是播放图标，点击后暂停
 */
extern "C" void action_toggle_play_pause(lv_event_t* e)
{
    std::cout << "[EEZ] Toggle play/pause" << std::endl;

    lv_obj_t* target = lv_event_get_target(e);
    const bool checked = target && lv_obj_has_state(target, LV_STATE_CHECKED);
    MusicActionService::getInstance().togglePlayPause(checked);
    refresh_music_playback_ui();
}

/**
 * @brief 播放下一首
 * @param e LVGL事件对象
 */
extern "C" void action_play_next(lv_event_t* e)
{
    std::cout << "[EEZ] Play next" << std::endl;
    MusicActionService::getInstance().playNext();
    refresh_music_page_ui();
}

/**
 * @brief 播放上一首
 * @param e LVGL事件对象
 */
extern "C" void action_play_prev(lv_event_t* e)
{
    std::cout << "[EEZ] Play previous" << std::endl;
    MusicActionService::getInstance().playPrev();
    refresh_music_page_ui();
}

/**
 * @brief 更新专辑封面
 * @param e LVGL事件对象
 * @details 获取当前歌曲的专辑封面并显示
 */
extern "C" void action_update_album_art(lv_event_t* e)
{
    std::cout << "[EEZ] Update album art" << std::endl;

    lv_obj_t* target = lv_event_get_target(e);
    if (!target)
        return;

    g_music_cover_path = MusicActionService::getInstance().getCurrentAlbumCoverPath();
    if (!g_music_cover_path.empty())
    {
        lv_img_set_src(target, g_music_cover_path.c_str());
    }
    else
    {
        lv_img_set_src(target, &img_music_image);
    }
}

/**
 * @brief 音量滑块值改变事件
 * @param e LVGL事件对象
 * @details 用户拖动音量滑块时，更新系统音量
 */
extern "C" void action_music_volume_changed(lv_event_t* e)
{
    if (!e || g_slider_syncing)
    {
        return;
    }

    const lv_event_code_t eventCode = lv_event_get_code(e);
    std::cout << "[EEZ] action_music_volume_changed event=" << static_cast<int>(eventCode) << std::endl;

    lv_obj_t* target = lv_event_get_target(e);
    if (!target || target != objects.music_volume_slider)
    {
        std::cout << "[EEZ] action_music_volume_changed ignored: target mismatch" << std::endl;
        return;
    }

    const int value = lv_slider_get_value(target);
    std::cout << "[EEZ] volume slider value=" << value << std::endl;
    MusicActionService::getInstance().setVolume(value);
}

/**
 * @brief 进度滑块值改变事件
 * @param e LVGL事件对象
 * @details 用户拖动进度条时，跳转到对应播放位置
 */
extern "C" void action_music_progress_changed(lv_event_t* e)
{
    if (!e || g_slider_syncing)
    {
        return;
    }

    const lv_event_code_t eventCode = lv_event_get_code(e);
    std::cout << "[EEZ] action_music_progress_changed event=" << static_cast<int>(eventCode) << std::endl;

    lv_obj_t* target = lv_event_get_target(e);
    if (!target || target != objects.music_progress_slider)
    {
        std::cout << "[EEZ] action_music_progress_changed ignored: target mismatch" << std::endl;
        return;
    }

    const int value = lv_slider_get_value(target);
    const double total = MusicActionService::getInstance().getTotalDuration();
    std::cout << "[EEZ] progress slider value=" << value << ", total=" << total << std::endl;
    if (total > 0.0)
    {
        const double seekTime = (static_cast<double>(value) / 100.0) * total;
        std::cout << "[EEZ] seek to " << seekTime << " sec" << std::endl;
        MusicActionService::getInstance().seekTo(seekTime);
    }
    refresh_music_playback_ui();
}

// ============================================================================
// 天气相关动作
// ============================================================================

#include "../service/WeatherService.h"
#include "../service/TimeService.h"
#include "../data/CityData.h"

// 城市选择器状态
static int s_provIndex = 0;
static int s_cityIndex = 0;
static int s_countyIndex = 0;
static bool s_cityDataLoaded = false;

/**
 * @brief 刷新天气界面UI
 * @details 直接使用 WeatherService C++ 接口获取天气数据并更新到各个UI控件
 */
static void refresh_weather_ui()
{
    WeatherService& service = WeatherService::getInstance();
    CurrentWeather current = service.getCurrent();
    std::vector<ForecastWeather> forecast = service.getForecast();

    // 更新当前天气信息 - 只显示 市/区 格式
    if (objects.weather_city_label)
    {
        std::string cityStr = current.city;
        // 提取市/区部分，去掉省份
        size_t firstSlash = cityStr.find('/');
        if (firstSlash != std::string::npos)
        {
            cityStr = cityStr.substr(firstSlash + 1); // 从第一个/后开始
        }
        lv_label_set_text(objects.weather_city_label, cityStr.c_str());
    }

    if (objects.weather_temp_label)
    {
        std::string tempStr = std::to_string(current.temperature) + "°C";
        lv_label_set_text(objects.weather_temp_label, tempStr.c_str());
    }

    if (objects.weather_text_label)
    {
        lv_label_set_text(objects.weather_text_label, current.description.c_str());
    }

    if (objects.weather_wind_label)
    {
        std::string windStr = current.windDir + " " + std::to_string(current.windSpeed) + "级";
        lv_label_set_text(objects.weather_wind_label, windStr.c_str());
    }

    // 更新时间和日期（使用网络时间）
    TimeService& timeService = TimeService::getInstance();
    DateTime dt = timeService.getCurrentDateTime();

    if (objects.weather_time_label)
    {
        std::string timeStr = timeService.getTimeString();
        lv_label_set_text(objects.weather_time_label, timeStr.c_str());
    }

    if (objects.weather_data_label)
    {
        // 显示完整日期：2025-12-11 星期四
        std::string dateStr = timeService.getFullDateString();
        lv_label_set_text(objects.weather_data_label, dateStr.c_str());
        lv_label_set_long_mode(objects.weather_data_label, LV_LABEL_LONG_DOT);
        lv_obj_set_style_text_align(objects.weather_data_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(objects.weather_data_label);
    }

    // 更新日历显示当前日期
    if (objects.sys_calendar)
    {
        lv_calendar_set_today_date(objects.sys_calendar, dt.year, dt.month, dt.day);
        lv_calendar_set_showed_date(objects.sys_calendar, dt.year, dt.month);
    }

    // 更新当前天气图标
    if (objects.weather_icon_label)
    {
        const char* iconStr = WeatherService::getIconStr(current.iconCode);
        lv_label_set_text(objects.weather_icon_label, iconStr);
    }

    // 更新天气预报（3天）
    lv_obj_t* fcst_temp_labels[] = {objects.weather_fcst_temp_0_label, objects.weather_fcst_temp_1_label,
                                    objects.weather_fcst_temp_2_label};

    lv_obj_t* fcst_icon_labels[] = {objects.weather_fcst_icon_0_label, objects.weather_fcst_icon_1_label,
                                    objects.weather_fcst_icon_2_label};

    lv_obj_t* fcst_wind_labels[] = {objects.weather_fcst_wind_0_label, objects.weather_fcst_wind_1_label,
                                    objects.weather_fcst_wind_2_label};

    for (size_t i = 0; i < forecast.size() && i < 3; ++i)
    {
        // 更新温度
        if (fcst_temp_labels[i])
        {
            std::string tempStr =
                std::to_string(forecast[i].tempMin) + "°/" + std::to_string(forecast[i].tempMax) + "°";
            lv_label_set_text(fcst_temp_labels[i], tempStr.c_str());
        }

        // 更新天气图标
        if (fcst_icon_labels[i])
        {
            const char* iconStr = WeatherService::getIconStr(forecast[i].iconCode);
            lv_label_set_text(fcst_icon_labels[i], iconStr);
        }

        // 更新风力
        if (fcst_wind_labels[i])
        {
            lv_label_set_text(fcst_wind_labels[i], forecast[i].windDir.c_str());
        }
    }

    // 调试输出天气数据
}

/**
 * @brief 加载城市数据到滚轮控件
 * @details 只在进入天气页面时调用一次
 */
static void loadCityDataToRollers()
{
    if (s_cityDataLoaded)
        return;

    // 加载省份
    if (objects.weather_prov_roller)
    {
        std::string provNames = CityData::getProvinceNames();
        lv_roller_set_options(objects.weather_prov_roller, provNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_prov_roller, 0, LV_ANIM_OFF);
    }

    // 加载默认城市（第一个省的城市）
    if (objects.weather_city_roller)
    {
        std::string cityNames = CityData::getCityNames(0);
        lv_roller_set_options(objects.weather_city_roller, cityNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_city_roller, 0, LV_ANIM_OFF);
    }

    // 加载默认区县（第一个省第一个城市的区县）
    if (objects.weather_county_roller)
    {
        std::string countyNames = CityData::getCountyNames(0, 0);
        lv_roller_set_options(objects.weather_county_roller, countyNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_county_roller, 0, LV_ANIM_OFF);
    }

    s_cityDataLoaded = true;
    s_provIndex = 0;
    s_cityIndex = 0;
    s_countyIndex = 0;
}

/**
 * @brief 省份改变事件
 * @param e LVGL事件对象
 */
extern "C" void action_prov_changed(lv_event_t* e)
{
    if (!objects.weather_prov_roller)
        return;

    s_provIndex = lv_roller_get_selected(objects.weather_prov_roller);

    // 更新城市列表
    if (objects.weather_city_roller)
    {
        std::string cityNames = CityData::getCityNames(s_provIndex);
        lv_roller_set_options(objects.weather_city_roller, cityNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_city_roller, 0, LV_ANIM_OFF);
        s_cityIndex = 0;
    }

    // 更新区县列表（新省的第一个城市）
    if (objects.weather_county_roller)
    {
        std::string countyNames = CityData::getCountyNames(s_provIndex, s_cityIndex);
        lv_roller_set_options(objects.weather_county_roller, countyNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_county_roller, 0, LV_ANIM_OFF);
        s_countyIndex = 0;
    }
}

/**
 * @brief 城市改变事件
 * @param e LVGL事件对象
 */
extern "C" void action_city_changed(lv_event_t* e)
{
    if (!objects.weather_city_roller)
        return;

    s_cityIndex = lv_roller_get_selected(objects.weather_city_roller);

    // 更新区县列表
    if (objects.weather_county_roller)
    {
        std::string countyNames = CityData::getCountyNames(s_provIndex, s_cityIndex);
        lv_roller_set_options(objects.weather_county_roller, countyNames.c_str(), LV_ROLLER_MODE_NORMAL);
        lv_roller_set_selected(objects.weather_county_roller, 0, LV_ANIM_OFF);
        s_countyIndex = 0;
    }
}

/**
 * @brief 确认城市选择
 * @param e LVGL事件对象
 */
extern "C" void action_confirm_city_picker(lv_event_t* e)
{
    if (objects.weather_county_roller)
    {
        s_countyIndex = lv_roller_get_selected(objects.weather_county_roller);
    }

    // 获取选中的城市名称和代码
    std::string cityName = CityData::getSelectedCityName(s_provIndex, s_cityIndex, s_countyIndex);
    std::string cityCode = CityData::getSelectedCityCode(s_provIndex, s_cityIndex, s_countyIndex);

    // 更新天气服务城市
    WeatherService& weatherService = WeatherService::getInstance();
    weatherService.setCity(cityName);
    weatherService.setCityCode(cityCode);

    // 更新天气数据
    weatherService.update();

    // 刷新UI
    refresh_weather_ui();

    // 隐藏选择器
    if (objects.weather_city_picker_mask)
    {
        lv_obj_add_flag(objects.weather_city_picker_mask, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * @brief 天气界面时间刷新定时器回调
 * @param timer LVGL定时器对象
 * @details 每秒检查一次，只在分钟变化时更新显示，确保时间同步准确
 */
static void weather_time_timer_cb(lv_timer_t* timer)
{
    TimeService& timeService = TimeService::getInstance();
    DateTime dt = timeService.getCurrentDateTime();

    // 只在分钟变化时更新显示
    if (dt.minute != g_last_minute)
    {
        g_last_minute = dt.minute;

        if (objects.weather_time_label)
        {
            std::string timeStr = timeService.getTimeString();
            lv_label_set_text(objects.weather_time_label, timeStr.c_str());
        }
    }
}

/**
 * @brief 初始化天气界面
 * @param e LVGL事件对象
 * @details 直接使用 WeatherService C++ 接口获取天气数据并显示
 */
extern "C" void action_init_weather_screen(lv_event_t* e)
{
    // 初始化并同步网络时间
    TimeService& timeService = TimeService::getInstance();
    timeService.init("ntp.aliyun.com");
    timeService.syncTime();

    // 加载城市数据（只加载一次）
    loadCityDataToRollers();

    // 更新天气数据
    WeatherService::getInstance().update();

    // 显示天气数据
    refresh_weather_ui();

    // 创建时间刷新定时器（每秒检查一次，但只在分钟变化时更新显示）
    if (g_weather_time_timer == nullptr)
    {
        // 初始化当前分钟数，确保第一次能正确显示
        DateTime dt = timeService.getCurrentDateTime();
        g_last_minute = dt.minute;
        g_weather_time_timer = lv_timer_create(weather_time_timer_cb, 1000, nullptr);
    }
}

/**
 * @brief 反初始化天气界面
 * @param e LVGL事件对象
 */
extern "C" void action_deinit_weather_screen(lv_event_t* e)
{
    // 删除时间刷新定时器
    if (g_weather_time_timer)
    {
        lv_timer_del(g_weather_time_timer);
        g_weather_time_timer = nullptr;
    }
}

// ============================================================================
// 设置页面动作
// ============================================================================

/**
 * @brief 亮度设置滑块值变化回调
 * @param e LVGL事件对象
 */
extern "C" void action_setting_brightness_changed(lv_event_t* e)
{
    lv_obj_t* slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);

    // TODO: 实现亮度调节功能
    // 可以通过写入/sys/class/backlight/.../brightness来调节屏幕亮度
}

/**
 * @brief 音量设置滑块值变化回调
 * @param e LVGL事件对象
 */
extern "C" void action_setting_volume_changed(lv_event_t* e)
{
    lv_obj_t* slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);

    // TODO: 实现音量调节功能
    // 可以通过amixer或pulseaudio设置系统音量
}

// ============================================================================
// WiFi设置页面动作
// ============================================================================

/**
 * @brief WiFi列表项点击回调
 * @param e LVGL事件对象
 */
static void wifi_item_click_cb(lv_event_t* e)
{
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 0);
    if (label)
    {
        const char* ssid = lv_label_get_text(label);
        g_selected_wifi_ssid = ssid;
        std::cout << "[WiFi] Selected: " << g_selected_wifi_ssid << std::endl;

        // 显示密码输入框
        if (objects.setting_wifi_secret_picker_mask)
        {
            lv_obj_clear_flag(objects.setting_wifi_secret_picker_mask, LV_OBJ_FLAG_HIDDEN);
            std::cout << "[WiFi] Password picker shown" << std::endl;
        }
    }
}

/**
 * @brief 刷新WiFi列表
 * @details 扫描附近WiFi并更新列表显示
 */
static void refresh_wifi_list()
{
    std::cout << "[WiFi] refresh_wifi_list called" << std::endl;

    if (!objects.wifi_list_panel)
    {
        std::cout << "[WiFi] wifi_list_panel is null" << std::endl;
        return;
    }

    // 清除现有列表
    lv_obj_clean(objects.wifi_list_panel);
    std::cout << "[WiFi] panel cleaned" << std::endl;

    // 直接使用已有的扫描结果，不重新扫描（避免阻塞）
    FILE* fp = popen("wpa_cli scan_results 2>/dev/null", "r");
    if (!fp)
    {
        std::cout << "[WiFi] failed to run wpa_cli scan_results" << std::endl;
        return;
    }

    char line[512];
    int y_offset = 0;
    int line_num = 0;
    int wifi_count = 0;

    std::cout << "[WiFi] reading scan results..." << std::endl;

    while (fgets(line, sizeof(line), fp))
    {
        line_num++;
        std::cout << "[WiFi] line " << line_num << ": " << line;

        // 跳过前两行（Selected interface 和 表头）
        if (line_num <= 2)
            continue;

        // 提取最后一个制表符后的字段作为SSID
        std::string line_str(line);
        // 找到最后一个制表符
        size_t last_tab = line_str.find_last_of('\t');
        if (last_tab != std::string::npos)
        {
            std::string ssid = line_str.substr(last_tab + 1);
            // 去除后面的换行符
            size_t end = ssid.find_last_not_of("\n\r");
            if (end != std::string::npos)
                ssid = ssid.substr(0, end + 1);

            std::cout << "[WiFi] extracted SSID: '" << ssid << "'" << std::endl;

            if (!ssid.empty() && ssid != "ssid")
            {
                // 创建WiFi项按钮
                lv_obj_t* btn = lv_btn_create(objects.wifi_list_panel);
                lv_obj_set_size(btn, lv_pct(90), 40);
                lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, y_offset);
                lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), 0);
                lv_obj_set_style_bg_opa(btn, LV_OPA_50, 0);

                // 创建SSID标签
                lv_obj_t* label = lv_label_create(btn);
                lv_label_set_text(label, ssid.c_str());
                lv_obj_center(label);

                // 添加点击事件
                lv_obj_add_event_cb(btn, wifi_item_click_cb, LV_EVENT_CLICKED, nullptr);

                y_offset += 45;
                wifi_count++;

                std::cout << "[WiFi] created button for: " << ssid << std::endl;

                if (y_offset > 400) // 限制显示数量
                    break;
            }
        }
    }

    pclose(fp);
    std::cout << "[WiFi] total WiFi found: " << wifi_count << std::endl;
}

/**
 * @brief 获取当前连接的WiFi信息
 * @param ssid 输出SSID
 * @param ip 输出IP地址
 * @return 是否成功获取
 */
static bool get_connected_wifi_info(std::string& ssid, std::string& ip)
{
    std::cout << "[WiFi] get_connected_wifi_info called" << std::endl;

    // 获取IP地址 - 适配ifconfig格式: inet addr:192.168.1.86
    FILE* fp = popen("ifconfig wlan0 2>/dev/null | grep 'inet addr:' | awk -F: '{print $2}' | awk '{print $1}'", "r");
    if (fp)
    {
        char buf[64];
        if (fgets(buf, sizeof(buf), fp))
        {
            buf[strcspn(buf, "\n")] = 0;
            ip = buf;
            std::cout << "[WiFi] IP found: '" << ip << "'" << std::endl;
        }
        else
        {
            std::cout << "[WiFi] IP not found" << std::endl;
        }
        pclose(fp);
    }

    // 获取SSID - 使用wpa_cli
    fp = popen("wpa_cli status 2>/dev/null | grep '^ssid=' | cut -d= -f2", "r");
    if (fp)
    {
        char buf[64];
        if (fgets(buf, sizeof(buf), fp))
        {
            buf[strcspn(buf, "\n")] = 0;
            ssid = buf;
            std::cout << "[WiFi] SSID found: '" << ssid << "'" << std::endl;
        }
        else
        {
            std::cout << "[WiFi] SSID not found" << std::endl;
        }
        pclose(fp);
    }

    bool result = !ip.empty() && !ssid.empty();
    std::cout << "[WiFi] get_connected_wifi_info result: " << (result ? "true" : "false") << std::endl;
    return result;
}

/**
 * @brief 初始化WiFi设置页面
 * @param e LVGL事件对象
 * @details 进入页面时检查WiFi状态，如果已连接则开关设为打开并显示WiFi列表和连接信息
 */
extern "C" void action_init_wifi_setting_page(lv_event_t* e)
{
    std::cout << "[WiFi] action_init_wifi_setting_page called" << std::endl;

    std::string ssid, ip;
    bool is_connected = get_connected_wifi_info(ssid, ip);
    std::cout << "[WiFi] is_connected: " << (is_connected ? "yes" : "no") << ", ssid: '" << ssid << "', ip: '" << ip
              << "'" << std::endl;

    // 根据实际WiFi状态设置开关
    if (objects.wifi_switch)
    {
        std::cout << "[WiFi] wifi_switch exists" << std::endl;
        if (is_connected)
        {
            lv_obj_add_state(objects.wifi_switch, LV_STATE_CHECKED);
            std::cout << "[WiFi] switch set to ON" << std::endl;
        }
        else
        {
            lv_obj_clear_state(objects.wifi_switch, LV_STATE_CHECKED);
            std::cout << "[WiFi] switch set to OFF" << std::endl;
        }
    }
    else
    {
        std::cout << "[WiFi] wifi_switch is null" << std::endl;
    }

    // 刷新WiFi列表（无论是否连接都显示）
    refresh_wifi_list();

    // 显示当前连接的WiFi信息（一行显示，支持循环滑动）
    if (objects.connected_wifi_info_label)
    {
        std::cout << "[WiFi] connected_wifi_info_label exists" << std::endl;
        if (is_connected)
        {
            std::string info = ssid + "  " + ip;
            lv_label_set_text(objects.connected_wifi_info_label, info.c_str());
        }
        else
        {
            lv_label_set_text(objects.connected_wifi_info_label, "未连接");
        }
    }
}

/**
 * @brief WiFi开关状态变化回调
 * @param e LVGL事件对象
 */
extern "C" void action_setting_wifi_state_changed(lv_event_t* e)
{
    lv_obj_t* sw = lv_event_get_target(e);
    bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);

    if (is_on)
    {
        // 打开WiFi
        std::cout << "[WiFi] Turning ON..." << std::endl;

        // 1. 检查并启动 wpa_supplicant
        std::cout << "[WiFi] Checking wpa_supplicant..." << std::endl;
        int ret = system("pgrep wpa_supplicant > /dev/null 2>&1");
        if (ret != 0)
        {
            std::cout << "[WiFi] Starting wpa_supplicant..." << std::endl;
            system("wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf > /dev/null 2>&1");
            sleep(2);
        }
        else
        {
            std::cout << "[WiFi] wpa_supplicant already running" << std::endl;
        }

        // 2. 启动 WiFi 接口
        std::cout << "[WiFi] Bringing up wlan0..." << std::endl;
        system("ifconfig wlan0 up > /dev/null 2>&1");
        sleep(1);

        // 3. 检查接口状态
        std::cout << "[WiFi] Checking interface status..." << std::endl;
        system("ifconfig wlan0");

        // 4. 启动扫描
        std::cout << "[WiFi] Starting scan..." << std::endl;
        ret = system("wpa_cli scan > /dev/null 2>&1");
        std::cout << "[WiFi] wpa_cli scan returned: " << ret << std::endl;

        // 5. 等待扫描完成（3秒）
        sleep(3);

        // 6. 检查扫描结果
        std::cout << "[WiFi] Checking scan results..." << std::endl;
        system("wpa_cli scan_results | head -5");

        // 7. 刷新WiFi列表显示
        std::cout << "[WiFi] Refreshing list..." << std::endl;
        refresh_wifi_list();
    }
    else
    {
        // 关闭WiFi
        std::cout << "[WiFi] Turning OFF..." << std::endl;
        system("ifconfig wlan0 down > /dev/null 2>&1");

        // 清空WiFi列表
        if (objects.wifi_list_panel)
        {
            lv_obj_clean(objects.wifi_list_panel);
        }
    }
}

/**
 * @brief 确认WiFi密码并连接
 * @param e LVGL事件对象
 * @details 获取密码输入框内容，配置wpa_supplicant并连接WiFi
 */
extern "C" void action_confirm_wifi_password(lv_event_t* e)
{
    std::cout << "[WiFi] action_confirm_wifi_password called" << std::endl;
    std::cout << "[WiFi] Selected SSID: " << g_selected_wifi_ssid << std::endl;

    // 获取密码
    const char* password = "";
    if (objects.setting_wifi_secret)
    {
        password = lv_textarea_get_text(objects.setting_wifi_secret);
        std::cout << "[WiFi] Password length: " << strlen(password) << std::endl;
    }

    if (g_selected_wifi_ssid.empty())
    {
        std::cout << "[WiFi] Error: No SSID selected" << std::endl;
        return;
    }

    // 创建wpa_supplicant配置文件
    std::string config_cmd = "wpa_cli remove_network all > /dev/null 2>&1; ";
    config_cmd += "wpa_cli add_network > /dev/null 2>&1; ";
    config_cmd += "wpa_cli set_network 0 ssid '\"" + g_selected_wifi_ssid + "\"' > /dev/null 2>&1; ";
    config_cmd += "wpa_cli set_network 0 psk '\"" + std::string(password) + "\"' > /dev/null 2>&1; ";
    config_cmd += "wpa_cli set_network 0 key_mgmt WPA-PSK > /dev/null 2>&1; ";
    config_cmd += "wpa_cli enable_network 0 > /dev/null 2>&1; ";
    config_cmd += "wpa_cli save_config > /dev/null 2>&1";

    std::cout << "[WiFi] Executing connection command..." << std::endl;
    int ret = system(config_cmd.c_str());

    if (ret == 0)
    {
        std::cout << "[WiFi] WiFi configuration saved, connecting..." << std::endl;

        // 触发连接
        system("wpa_cli select_network 0 > /dev/null 2>&1");

        // 等待WiFi连接完成（最多10秒）
        bool wifi_connected = false;
        for (int i = 0; i < 10; i++)
        {
            sleep(1);
            std::string check_ssid, check_ip;
            if (get_connected_wifi_info(check_ssid, check_ip))
            {
                if (check_ssid == g_selected_wifi_ssid)
                {
                    wifi_connected = true;
                    break;
                }
            }
            // 检查是否已连接（有SSID但可能没有IP）
            FILE* fp = popen("wpa_cli status 2>/dev/null | grep '^ssid=' | cut -d= -f2", "r");
            if (fp)
            {
                char buf[64];
                if (fgets(buf, sizeof(buf), fp))
                {
                    buf[strcspn(buf, "\n")] = 0;
                    if (g_selected_wifi_ssid == buf)
                    {
                        wifi_connected = true;
                    }
                }
                pclose(fp);
            }
            if (wifi_connected)
                break;
        }

        if (wifi_connected)
        {
            std::cout << "[WiFi] WiFi connected, getting IP via DHCP..." << std::endl;

            // 启动 DHCP 客户端获取 IP
            system("udhcpc -i wlan0 -q > /dev/null 2>&1");
            sleep(2);

            // 再次获取连接信息（应该有IP了）
            std::string check_ssid, check_ip;
            if (get_connected_wifi_info(check_ssid, check_ip) && !check_ip.empty())
            {
                std::cout << "[WiFi] Connected successfully to: " << check_ssid << " IP: " << check_ip << std::endl;

                // 更新UI
                if (objects.connected_wifi_info_label)
                {
                    std::string info = check_ssid + "  " + check_ip;
                    lv_label_set_text(objects.connected_wifi_info_label, info.c_str());
                }

                // 隐藏密码输入框
                if (objects.setting_wifi_secret_picker_mask)
                {
                    lv_obj_add_flag(objects.setting_wifi_secret_picker_mask, LV_OBJ_FLAG_HIDDEN);
                }

                // 刷新WiFi列表
                refresh_wifi_list();
                return;
            }
            else
            {
                std::cout << "[WiFi] Connected but failed to get IP" << std::endl;
            }
        }

        std::cout << "[WiFi] Connection timeout" << std::endl;
    }
    else
    {
        std::cout << "[WiFi] Failed to configure WiFi" << std::endl;
    }
}