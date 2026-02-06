# rk3506-lvgl-app

这是一个基于瑞芯微 RK3506 平台的 LVGL 综合应用项目。

## 📁 目录结构说明

本项目包含以下主要目录和模块：

- **common/**: 通用工具与算法
  - 包含卡尔曼滤波 (`kalman_filter`) 和 UI 缩放 (`ui_scaler`) 等通用组件。

- **lvgl8/**: LVGL 移植层
  - 包含输入设备驱动 (`evdev`, `key`) 及 LVGL 显示与输入接口配置 (`lv_port_disp`, `lv_port_indev`)。

- **src/**: 核心业务逻辑
  - **bridge/**: 应用程序桥接层 (`app_bridge`)。
  - **fonts/**: 字体与图标资源。
  - **hal/**: 硬件抽象层，封装底层硬件操作（音频、背光、DHT11 传感器、媒体解码等）。
  - **service/**: 后台服务模块（MQTT 通信、音乐播放器、天气服务）。

- **sys/**: 系统底层接口
  - 包含 CPU、SOC 信息、时间戳 (`timestamp`) 及调试追踪 (`trace`) 功能。

- **ui/**: 用户界面 (GUI)
  - **custom/**: 用户自定义的 UI 逻辑与中文字库支持。
  - **generated/**: 由 GUI 工具 (如 NXP GUI Guider) 自动生成的代码。

## 🛠️ 构建与编译

本项目使用 CMake 进行构建。

```bash
mkdir build
cd build
cmake ..
make
```

## ✨ 功能特性

- **多媒体支持**: 集成音频播放与媒体解码。
- **传感器集成**: 支持 DHT11 温湿度传感器数据读取。
- **网络服务**: 内置 MQTT 客户端与实时天气服务。
- **流畅交互**: 基于 LVGL 8 的现代化图形用户界面。

---
*注：本项目作为 SDK 的一部分运行在 Luckfox RK3506 开发板上。*
