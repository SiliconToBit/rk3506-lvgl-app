#!/bin/bash

# 红外测试程序编译脚本

set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="${SCRIPT_DIR}/build"

echo "======================================"
echo "红外学习模块测试程序编译"
echo "======================================"

# 创建构建目录
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# 检测交叉编译器
if command -v arm-buildroot-linux-gnueabihf-g++ &> /dev/null; then
    echo "使用交叉编译器: arm-buildroot-linux-gnueabihf-g++"
    cmake .. \
        -DCMAKE_CXX_COMPILER=arm-buildroot-linux-gnueabihf-g++ \
        -DCMAKE_CXX_FLAGS="-std=c++17"
elif command -v arm-linux-gnueabihf-g++ &> /dev/null; then
    echo "使用交叉编译器: arm-linux-gnueabihf-g++"
    cmake .. \
        -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
        -DCMAKE_CXX_FLAGS="-std=c++17"
else
    echo "警告: 未找到交叉编译器，使用系统默认 g++"
    echo "注意: 这将在当前平台(PC)上编译，可能无法在RK3506上运行"
    cmake .. -DCMAKE_CXX_FLAGS="-std=c++17"
fi

# 编译
make -j$(nproc)

echo ""
echo "======================================"
echo "编译完成!"
echo "输出文件: ${BUILD_DIR}/test_ir"
echo "======================================"
echo ""
echo "使用方法:"
echo "  ./test_ir /dev/ttyS1              # 交互式模式"
echo "  ./test_ir /dev/ttyS1 learn 0      # 学习红外码到索引0"
echo "  ./test_ir /dev/ttyS1 emit 0       # 发射索引0的红外码"
echo "  ./test_ir /dev/ttyS1 clearall     # 清除所有红外码"
