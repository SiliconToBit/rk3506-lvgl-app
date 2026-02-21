#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

static const char *screen_names[] = { "Main", "P1", "P2", "WeatherPage", "SensorPage", "SettingPage", "CpuPage", "ImagePage", "VideoPage", "CameraPage", "MusicPage" };
static const char *object_names[] = { "main", "p1", "p2", "weather_page", "sensor_page", "setting_page", "cpu_page", "image_page", "video_page", "camera_page", "music_page", "setting_btn", "weather_btn", "sensor_btn", "music_btn", "image_btn", "cpu_btn", "video_btn", "camera_btn", "setting_btn_1", "weather_time_panel", "temp_label", "weather_icon_label", "winddir_label", "weather_text_label", "time_location_panel", "weather_forecast_panel" };

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

static void event_handler_cb_main_main(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_GESTURE) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 1, 0, e);
    }
}

static void event_handler_cb_p1_p1(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_GESTURE) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 1, 0, e);
    }
}

static void event_handler_cb_p1_weather_btn(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_CLICKED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 2, 0, e);
    }
}

static void event_handler_cb_p2_p2(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_GESTURE) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 1, 0, e);
    }
}

static void event_handler_cb_weather_page_weather_page(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_GESTURE) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 1, 0, e);
    }
}

//
// Screens
//

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    lv_obj_add_event_cb(obj, event_handler_cb_main_main, LV_EVENT_ALL, flowState);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 338, 195);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_64, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "12:12");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
}

void create_screen_p1() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.p1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    lv_obj_add_event_cb(obj, event_handler_cb_p1_p1, LV_EVENT_ALL, flowState);
    {
        lv_obj_t *parent_obj = obj;
        {
            // setting_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.setting_btn = obj;
            lv_obj_set_pos(obj, 63, 70);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_setting4, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // weather_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.weather_btn = obj;
            lv_obj_set_pos(obj, 267, 70);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_add_event_cb(obj, event_handler_cb_p1_weather_btn, LV_EVENT_ALL, flowState);
            lv_obj_set_style_bg_img_src(obj, &img_weather, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff00f0ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // sensor_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.sensor_btn = obj;
            lv_obj_set_pos(obj, 676, 70);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_sensor, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff2a632c), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // music_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.music_btn = obj;
            lv_obj_set_pos(obj, 267, 284);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_music, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // image_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.image_btn = obj;
            lv_obj_set_pos(obj, 63, 284);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_image, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff237cff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // cpu_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.cpu_btn = obj;
            lv_obj_set_pos(obj, 472, 70);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_cpu, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffc5c5c5), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // video_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.video_btn = obj;
            lv_obj_set_pos(obj, 472, 284);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_video2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // camera_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.camera_btn = obj;
            lv_obj_set_pos(obj, 676, 284);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_camera, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_p1();
}

void tick_screen_p1() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}

void create_screen_p2() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.p2 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    lv_obj_add_event_cb(obj, event_handler_cb_p2_p2, LV_EVENT_ALL, flowState);
    {
        lv_obj_t *parent_obj = obj;
        {
            // setting_btn_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.setting_btn_1 = obj;
            lv_obj_set_pos(obj, 63, 70);
            lv_obj_set_size(obj, 128, 128);
            lv_obj_set_style_bg_img_src(obj, &img_doubao, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 26, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_p2();
}

void tick_screen_p2() {
    void *flowState = getFlowState(0, 2);
    (void)flowState;
}

void create_screen_weather_page() {
    void *flowState = getFlowState(0, 3);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.weather_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    lv_obj_add_event_cb(obj, event_handler_cb_weather_page_weather_page, LV_EVENT_ALL, flowState);
    {
        lv_obj_t *parent_obj = obj;
        {
            // weather_time_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.weather_time_panel = obj;
            lv_obj_set_pos(obj, 20, 20);
            lv_obj_set_size(obj, 400, 200);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // temp_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.temp_label = obj;
                    lv_obj_set_pos(obj, 222, 12);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "26℃");
                }
                {
                    // weather_icon_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.weather_icon_label = obj;
                    lv_obj_set_pos(obj, LV_PCT(1), LV_PCT(1));
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_qweather_icons_128, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    // winddir_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.winddir_label = obj;
                    lv_obj_set_pos(obj, 200, 58);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "东风二级");
                }
                {
                    // weather_text_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.weather_text_label = obj;
                    lv_obj_set_pos(obj, 152, 111);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "大暴雨到特大暴雨");
                }
            }
        }
        {
            // time_location_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.time_location_panel = obj;
            lv_obj_set_pos(obj, 445, 20);
            lv_obj_set_size(obj, 400, 200);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 67, 78);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2025-12-11 星期四");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 83, -19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_64, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "12:00");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 109, 120);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "广州番禺");
                }
            }
        }
        {
            // weather_forecast_panel
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.weather_forecast_panel = obj;
            lv_obj_set_pos(obj, 20, 240);
            lv_obj_set_size(obj, 452, 230);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 114, 327);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_qweather_icons_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    lv_obj_t *obj = lv_line_create(parent_obj);
                    lv_obj_set_pos(obj, -22, 29);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    static lv_point_t line_points[] = {
                        { 0, 0 },
                        { 400, 0 }
                    };
                    lv_line_set_points(obj, line_points, 2);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -16, -15);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "3日天气预报");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -11, 36);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "今天");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -11, 93);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "明天");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -11, 151);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "后天");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 91, 41);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_qweather_icons_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 91, 98);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_qweather_icons_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 92, 156);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_qweather_icons_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 161, 36);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "24℃-26℃");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 162, 93);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "24℃-26℃");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 162, 146);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "24℃-26℃");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 353, 36);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "南风");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 354, 93);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "南风");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 355, 147);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_font(obj, ui_font_alibaba_pu_hui_ti_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "南风");
                }
            }
        }
        {
            lv_obj_t *obj = lv_calendar_create(parent_obj);
            lv_obj_set_pos(obj, 487, 240);
            lv_obj_set_size(obj, 358, 230);
            lv_calendar_header_arrow_create(obj);
            lv_calendar_set_today_date(obj, 2022, 11, 1);
            lv_calendar_set_showed_date(obj, 2022, 11);
        }
    }
    
    tick_screen_weather_page();
}

void tick_screen_weather_page() {
    void *flowState = getFlowState(0, 3);
    (void)flowState;
}

void create_screen_sensor_page() {
    void *flowState = getFlowState(0, 4);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.sensor_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_sensor_page();
}

void tick_screen_sensor_page() {
    void *flowState = getFlowState(0, 4);
    (void)flowState;
}

void create_screen_setting_page() {
    void *flowState = getFlowState(0, 5);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_setting_page();
}

void tick_screen_setting_page() {
    void *flowState = getFlowState(0, 5);
    (void)flowState;
}

void create_screen_cpu_page() {
    void *flowState = getFlowState(0, 6);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.cpu_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_cpu_page();
}

void tick_screen_cpu_page() {
    void *flowState = getFlowState(0, 6);
    (void)flowState;
}

void create_screen_image_page() {
    void *flowState = getFlowState(0, 7);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.image_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_image_page();
}

void tick_screen_image_page() {
    void *flowState = getFlowState(0, 7);
    (void)flowState;
}

void create_screen_video_page() {
    void *flowState = getFlowState(0, 8);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.video_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_video_page();
}

void tick_screen_video_page() {
    void *flowState = getFlowState(0, 8);
    (void)flowState;
}

void create_screen_camera_page() {
    void *flowState = getFlowState(0, 9);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.camera_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_camera_page();
}

void tick_screen_camera_page() {
    void *flowState = getFlowState(0, 9);
    (void)flowState;
}

void create_screen_music_page() {
    void *flowState = getFlowState(0, 10);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.music_page = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 854, 480);
    
    tick_screen_music_page();
}

void tick_screen_music_page() {
    void *flowState = getFlowState(0, 10);
    (void)flowState;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_p1,
    tick_screen_p2,
    tick_screen_weather_page,
    tick_screen_sensor_page,
    tick_screen_setting_page,
    tick_screen_cpu_page,
    tick_screen_image_page,
    tick_screen_video_page,
    tick_screen_camera_page,
    tick_screen_music_page,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

//
// Fonts
//

lv_font_t *ui_font_qweather_icons_32;
lv_font_t *ui_font_qweather_icons_64;
lv_font_t *ui_font_qweather_icons_128;
lv_font_t *ui_font_alibaba_pu_hui_ti_16;
lv_font_t *ui_font_alibaba_pu_hui_ti_24;
lv_font_t *ui_font_alibaba_pu_hui_ti_32;
lv_font_t *ui_font_alibaba_pu_hui_ti_64;

ext_font_desc_t fonts[] = {
    { "qweather-icons-32", NULL },
    { "qweather-icons-64", NULL },
    { "qweather-icons-128", NULL },
    { "AlibabaPuHuiTi-16", NULL },
    { "AlibabaPuHuiTi-24", NULL },
    { "AlibabaPuHuiTi-32", NULL },
    { "AlibabaPuHuiTi-64", NULL },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
//
//

void create_screens() {
    // Load external fonts
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/qweather-icons.ttf";
        info.weight = 32;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_qweather_icons_32 = info.font;
            fonts[0].font_ptr = ui_font_qweather_icons_32;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_qweather_icons_32");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/qweather-icons.ttf";
        info.weight = 64;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_qweather_icons_64 = info.font;
            fonts[1].font_ptr = ui_font_qweather_icons_64;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_qweather_icons_64");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/qweather-icons.ttf";
        info.weight = 128;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_qweather_icons_128 = info.font;
            fonts[2].font_ptr = ui_font_qweather_icons_128;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_qweather_icons_128");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf";
        info.weight = 16;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_alibaba_pu_hui_ti_16 = info.font;
            fonts[3].font_ptr = ui_font_alibaba_pu_hui_ti_16;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_alibaba_pu_hui_ti_16");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf";
        info.weight = 24;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_alibaba_pu_hui_ti_24 = info.font;
            fonts[4].font_ptr = ui_font_alibaba_pu_hui_ti_24;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_alibaba_pu_hui_ti_24");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf";
        info.weight = 32;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_alibaba_pu_hui_ti_32 = info.font;
            fonts[5].font_ptr = ui_font_alibaba_pu_hui_ti_32;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_alibaba_pu_hui_ti_32");
        }
    }
    {
        lv_ft_info_t info;
        info.name = "/usr/share/fonts/AlibabaPuHuiTi-3-115-Black.ttf";
        info.weight = 64;
        info.style = FT_FONT_STYLE_NORMAL;
        info.mem = 0;
        if (lv_ft_font_init(&info)) {
            ui_font_alibaba_pu_hui_ti_64 = info.font;
            fonts[6].font_ptr = ui_font_alibaba_pu_hui_ti_64;
        } else {
            LV_LOG_ERROR("font create failed: ui_font_alibaba_pu_hui_ti_64");
        }
    }
    
    eez_flow_init_fonts(fonts, sizeof(fonts) / sizeof(ext_font_desc_t));

// Set default LVGL theme
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    // Initialize screens
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    // Create screens
    create_screen_main();
    create_screen_p1();
    create_screen_p2();
    create_screen_weather_page();
    create_screen_sensor_page();
    create_screen_setting_page();
    create_screen_cpu_page();
    create_screen_image_page();
    create_screen_video_page();
    create_screen_camera_page();
    create_screen_music_page();
}