#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_setting4;
extern const lv_img_dsc_t img_weather;
extern const lv_img_dsc_t img_cpu;
extern const lv_img_dsc_t img_sensor;
extern const lv_img_dsc_t img_image;
extern const lv_img_dsc_t img_music;
extern const lv_img_dsc_t img_video;
extern const lv_img_dsc_t img_video2;
extern const lv_img_dsc_t img_camera;
extern const lv_img_dsc_t img_doubao;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[10];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/