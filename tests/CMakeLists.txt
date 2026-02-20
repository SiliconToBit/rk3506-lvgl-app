cmake_minimum_required(VERSION 3.10)
project(lvgl_app_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 获取父目录的源文件路径
set(PARENT_DIR ${CMAKE_SOURCE_DIR}/..)

# 头文件路径
include_directories(
    ${PARENT_DIR}/src
    ${PARENT_DIR}/src/hal
)

# 红外模块测试
add_executable(test_ir
    test_ir.cpp
    ${PARENT_DIR}/src/hal/IRDevice.cpp
)

# 红外命令管理测试
add_executable(test_ir_cmd
    test_ir_cmd.cpp
    ${PARENT_DIR}/src/hal/IRDevice.cpp
    ${PARENT_DIR}/src/hal/IRCommandManager.cpp
)

# 打印信息
message(STATUS "Tests build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Parent directory: ${PARENT_DIR}")
