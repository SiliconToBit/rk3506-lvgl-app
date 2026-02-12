cmake_minimum_required(VERSION 3.10)

# 项目名称和语言设置
project(lvgl_demo LANGUAGES C CXX)

# C 语言标准设置：使用 C11 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# C++ 语言标准设置：使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 工具链配置 ---
# 设置交叉编译工具链目录（RK3506 平台）
set(TOOLCHAIN_DIR $ENV{RK3506_TOOLCHAIN_DIR})
if(NOT TOOLCHAIN_DIR)
    set(TOOLCHAIN_DIR /home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/buildroot/output/rockchip_rk3506_luckfox/host)
endif()

# 设置编译器路径
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-gcc)

# 设置系统根目录
set(CMAKE_SYSROOT ${TOOLCHAIN_DIR}/arm-buildroot-linux-gnueabihf/sysroot)

# --- 头文件搜索路径 ---
include_directories(
    ${CMAKE_SYSROOT}/usr/include/lvgl
    ${CMAKE_SYSROOT}/usr/include/lvgl/lv_drivers
    ${CMAKE_SYSROOT}/usr/include/c++/12.4.0
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/hal
    ${PROJECT_SOURCE_DIR}/src/service
    ${PROJECT_SOURCE_DIR}/src/bridge
    ${PROJECT_SOURCE_DIR}/src/fonts
    ${PROJECT_SOURCE_DIR}/lvgl8
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/sys
    ${PROJECT_SOURCE_DIR}/ui/custom
    ${PROJECT_SOURCE_DIR}/ui/generated
    ${PROJECT_SOURCE_DIR}/ui/generated/guider_customer_fonts
)

# --- 宏定义 ---
add_definitions(-DUSE_RK3506=1)
add_definitions(-DUSE_DRM=1 -DUSE_EVDEV=1 -DUSE_RGA=1)

# --- 编译器标志 ---
# 禁用 GCC 7.1 ABI 变更警告（psabi）
add_compile_options(-Wno-psabi)

# --- 收集所有源文件 ---
aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl8 SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/sys SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/common SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/custom SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated/guider_fonts SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated/images SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/hal SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/service SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/bridge SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/fonts SRCS)

# 只添加主程序文件
list(APPEND SRCS ${PROJECT_SOURCE_DIR}/main.cpp)

# --- 创建可执行文件 ---
add_executable(${PROJECT_NAME} ${SRCS})

# --- 链接外部库 ---
# LVGL 图形库及其驱动
# freetype: 字体渲染库
# drm: Direct Rendering Manager（显示管理）
# evdev: Linux 输入事件设备
# rga: Rockchip 硬件加速
# pthread: 多线程支持
# asound: ALSA 音频库
# avformat/avcodec/swscale/swresample: FFmpeg 音视频编解码
# curl: HTTP 客户端库
# paho-mqttpp3/paho-mqtt3c: MQTT 客户端库
target_link_libraries(${PROJECT_NAME}
    lvgl lv_drivers freetype drm evdev rga pthread asound
    avformat avcodec avutil swscale swresample m
    curl paho-mqttpp3 paho-mqtt3c
)

# --- 自定义清理目标 ---
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove Makefile
    COMMAND ${CMAKE_COMMAND} -E remove_directory CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_NAME}
    COMMENT "Clean all CMake generated files and ${PROJECT_NAME}"
)

# --- 安装配置 ---
set(OVERLAY_ROOT_DIR $ENV{RK3506_OVERLAY_DIR})
if(NOT OVERLAY_ROOT_DIR)
    set(OVERLAY_ROOT_DIR "/home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/overlay/root")
endif()

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${OVERLAY_ROOT_DIR}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)
