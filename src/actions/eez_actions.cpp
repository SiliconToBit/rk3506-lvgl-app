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
