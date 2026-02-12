/**
 * @file main.h
 * @brief 主程序头文件
 */

#ifndef __MAIN_H__
#define __MAIN_H__

// 仅保留主程序必需的头文件
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "lv_port_init.h"
#include <lvgl/lvgl.h>

// 宏定义
#define ALIGN(x, a) (((x) + (a - 1)) & ~(a - 1))

#endif // __MAIN_H__
