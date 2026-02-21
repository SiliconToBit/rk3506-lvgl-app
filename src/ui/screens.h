#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_P1 = 2,
    SCREEN_ID_P2 = 3,
    SCREEN_ID_WEATHER_PAGE = 4,
    SCREEN_ID_SENSOR_PAGE = 5,
    SCREEN_ID_SETTING_PAGE = 6,
    SCREEN_ID_CPU_PAGE = 7,
    SCREEN_ID_IMAGE_PAGE = 8,
    SCREEN_ID_VIDEO_PAGE = 9,
    SCREEN_ID_CAMERA_PAGE = 10,
    SCREEN_ID_MUSIC_PAGE = 11,
    _SCREEN_ID_LAST = 11
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *p1;
    lv_obj_t *p2;
    lv_obj_t *weather_page;
    lv_obj_t *sensor_page;
    lv_obj_t *setting_page;
    lv_obj_t *cpu_page;
    lv_obj_t *image_page;
    lv_obj_t *video_page;
    lv_obj_t *camera_page;
    lv_obj_t *music_page;
    lv_obj_t *setting_btn;
    lv_obj_t *weather_btn;
    lv_obj_t *sensor_btn;
    lv_obj_t *music_btn;
    lv_obj_t *image_btn;
    lv_obj_t *cpu_btn;
    lv_obj_t *video_btn;
    lv_obj_t *camera_btn;
    lv_obj_t *setting_btn_1;
    lv_obj_t *weather_time_panel;
    lv_obj_t *temp_label;
    lv_obj_t *weather_icon_label;
    lv_obj_t *winddir_label;
    lv_obj_t *weather_text_label;
    lv_obj_t *time_location_panel;
    lv_obj_t *weather_forecast_panel;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void create_screen_p1();
void tick_screen_p1();

void create_screen_p2();
void tick_screen_p2();

void create_screen_weather_page();
void tick_screen_weather_page();

void create_screen_sensor_page();
void tick_screen_sensor_page();

void create_screen_setting_page();
void tick_screen_setting_page();

void create_screen_cpu_page();
void tick_screen_cpu_page();

void create_screen_image_page();
void tick_screen_image_page();

void create_screen_video_page();
void tick_screen_video_page();

void create_screen_camera_page();
void tick_screen_camera_page();

void create_screen_music_page();
void tick_screen_music_page();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/