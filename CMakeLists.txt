#@author: Deng Zhimao
#@email: dengzhimao@alientek.com
#B站作品: https://space.bilibili.com/474103963?spm_id_from=333.788.0.0

#开源电子网: http://www.openedv.com/forum.php
#正点原子官网: https://www.alientek.com
#店铺: https://zhengdianyuanzi.tmall.com

#LICENCE GPLV3 
cmake_minimum_required(VERSION 3.10)

project(lvgl_demo LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TOOLCHAIN_DIR /home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/buildroot/output/rockchip_rk3506_luckfox/host)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-buildroot-linux-gnueabihf-gcc)
set(SYSROOT /home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/buildroot/output/rockchip_rk3506_luckfox/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include)
set(CMAKE_SYSROOT /home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/buildroot/output/rockchip_rk3506_luckfox/host/arm-buildroot-linux-gnueabihf/sysroot)


include_directories(
        ${CMAKE_SYSROOT}/usr/include/lvgl
        ${CMAKE_SYSROOT}/usr/include/lvgl/lv_drivers
        ${CMAKE_SYSROOT}/usr/include/c++/12.4.0
        )

add_definitions(-DUSE_RK3506=1)
add_definitions(-DUSE_DRM=1 -DUSE_EVDEV=1 -DUSE_RGA=1)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src/hal)
include_directories(${PROJECT_SOURCE_DIR}/src/service)
include_directories(${PROJECT_SOURCE_DIR}/src/bridge)
include_directories(${PROJECT_SOURCE_DIR}/src/fonts)
include_directories(${PROJECT_SOURCE_DIR}/lvgl8)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/sys)
include_directories(${PROJECT_SOURCE_DIR}/ui/custom)
include_directories(${PROJECT_SOURCE_DIR}/ui/generated)
include_directories(${PROJECT_SOURCE_DIR}/ui/generated/guider_customer_fonts)

aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl8 SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/sys SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/common SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/custom SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated/guider_fonts SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/ui/generated/images SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR} SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/hal SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/service SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/bridge SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/fonts SRCS)

add_executable(${PROJECT_NAME} ${SRCS}
        src/hal/Backlight.h
        src/hal/Backlight.cpp
        src/hal/MediaDecoder.cpp
        src/hal/MediaDecoder.h)

target_link_libraries(${PROJECT_NAME}
        lvgl lv_drivers freetype drm evdev rga pthread asound
        avformat avcodec avutil swscale swresample m
        curl paho-mqttpp3 paho-mqtt3c
)

add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove Makefile
    COMMAND ${CMAKE_COMMAND} -E remove_directory CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_NAME}
    COMMENT "Clean all CMake generated files and ${PROJECT_NAME}"
)

# --- Install Configuration ---

# Define the overlay paths
set(OVERLAY_ROOT_DIR "/home/gm/Workspace/linux_sdk/luckfox_rk3506_sdk/overlay/root")

# Install the executable to /root/
install(TARGETS ${PROJECT_NAME} 
        DESTINATION ${OVERLAY_ROOT_DIR}
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

