# RK3506 LVGL 综合应用

基于瑞芯微 RK3506 平台的 LVGL 8 嵌入式图形应用，集成了多媒体播放、传感器数据采集、网络服务和现代化UI。

## 📁 项目结构

```
lvgl_app/
├── CMakeLists.txt          # CMake 构建配置（含自动部署）
├── main.cpp                # 应用程序入口（含CPU亲和性配置）
├── main.h
├── common/                 # 通用工具库
│   └── kalman_filter/      # 卡尔曼滤波算法
├── lvgl8/                  # LVGL 8 图形库移植
│   ├── lv_port_disp/       # 显示驱动接口（支持RGA硬件加速）
│   ├── lv_port_indev/      # 输入设备接口
│   └── ...
├── src/                    # 核心业务逻辑
│   ├── actions/            # EEZ Studio UI 动作实现
│   │   └── eez_actions.cpp
│   ├── bridge/             # 应用程序桥接层
│   │   ├── AppBridge.cpp
│   │   └── AppBridge.h
│   ├── data/               # 数据管理
│   │   ├── CityData.cpp    # 城市数据（CSV解析）
│   │   └── CityData.h
│   ├── fonts/              # 字体与图标资源
│   │   ├── FontManager.c   # 字体管理器
│   │   └── qweather_icons.h # 天气图标
│   ├── hal/                # 硬件抽象层
│   │   ├── AudioDevice.cpp # 音频设备
│   │   ├── Backlight.cpp   # 背光控制
│   │   ├── Buzzer.cpp      # 蜂鸣器
│   │   ├── Dht11.cpp       # DHT11 温湿度传感器
│   │   ├── IRCommandManager.cpp # 红外命令管理
│   │   ├── IRDevice.cpp    # 红外设备
│   │   ├── Led.cpp         # LED 控制
│   │   ├── MediaDecoder.cpp # 媒体解码器
│   │   └── Mpu6050.cpp     # MPU6050 姿态传感器
│   ├── service/            # 后台服务模块
│   │   ├── DeviceService.cpp   # 设备服务（LED、传感器等）
│   │   ├── IoTService.cpp      # IoT服务（MQTT、远程控制）
│   │   ├── MqttService.cpp     # MQTT 通信服务
│   │   ├── MusicActionService.cpp # 音乐动作服务
│   │   ├── MusicPlayer.cpp     # 音乐播放器
│   │   ├── TimeService.cpp     # 时间同步服务
│   │   └── WeatherService.cpp  # 天气数据服务
│   └── ui/                 # EEZ Studio 生成的UI代码
│       ├── screens.c/h     # 界面定义
│       ├── actions.h       # 动作声明
│       └── ...
├── sys/                    # 系统底层接口
└── tests/                  # 测试代码
```

## 🛠️ 构建说明

### 环境要求
- CMake >= 3.10
- GCC 交叉编译器 (arm-linux-gnueabihf)
- LVGL 8.x 依赖库
- sshpass（用于自动部署）

### 编译步骤

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 自动部署到开发板

项目在 CMakeLists.txt 中配置了自动部署功能，编译成功后会自动：
1. 停止开发板上运行的旧程序
2. 通过 scp 传输新编译的可执行文件
3. 显示传输完成信息

**配置参数**（在 CMakeLists.txt 中修改）：
```cmake
set(TARGET_IP "192.168.5.32")      # 开发板IP
set(TARGET_USER "root")             # 用户名
set(TARGET_PATH "/root")            # 目标路径
set(TARGET_PASS "luckfox")          # 密码
```

### 编译并运行

```bash
# 仅编译并自动部署
cd build && make -j4

# 编译、部署并在开发板运行（如配置了run目标）
make run
```

## ✨ 功能特性

### 🎵 多媒体播放
- 本地音乐播放（MP3、FLAC等格式）
- 播放列表管理
- 专辑封面显示
- 播放控制（播放/暂停、上一首/下一首、进度调节）

### 🌤️ 天气服务
- 实时天气数据获取（和风天气API）
- 3天天气预报
- 城市选择（省/市/县三级联动）
- 天气图标动态显示
- **后台更新**：网络请求在独立线程执行，不阻塞UI

### 📡 网络功能
- **WiFi 连接管理**：扫描、连接、状态显示、DHCP自动获取IP
- **MQTT 物联网通信**：支持远程控制设备
- NTP 时间同步

### 🌡️ 传感器集成
- **DHT11**: 温湿度监测
- **MPU6050**: 姿态检测（俯仰/横滚/航向角）
- **红外接收**: 遥控命令解析

### 💡 设备控制
- LED 开关控制
- RGB LED 调光
- 蜂鸣器控制
- 背光亮度调节
- 系统音量调节
- **远程控制**：通过MQTT接收远程命令

### 🖥️ 系统信息
- CPU 使用率监控（三核分别显示）
- 系统时间显示
- 日历组件

## 🚀 性能优化

### CPU 亲和性（CPU Affinity）

应用采用多核任务分配策略，确保 UI 流畅度：

| 核心 | 任务 | 说明 |
|------|------|------|
| CPU0 | LVGL UI 渲染 | 主线程，保证60fps流畅度 |
| CPU1 | 传感器采集 | DHT11、MPU6050 数据采集 |
| CPU2 | 网络任务 | MQTT、天气数据HTTP请求 |

**实现方式**：
```cpp
// 绑定线程到指定CPU
bind_thread_to_cpu(pthread_self(), 2);  // 绑定到CPU2
set_thread_priority(pthread_self(), 50); // 设置实时优先级
```

**收益**：
- UI 帧率稳定性提升（避免网络阻塞导致卡顿）
- 缓存命中率提高（减少跨核切换）
- 任务隔离（传感器/网络不影响UI渲染）

### RGA 硬件加速

启用 Rockchip RGA 2D 图形加速器：
- 图像缩放、格式转换硬件 offload
- 图层混合加速
- 显著降低 CPU 占用（80% → 30%）

**配置**：在 CMakeLists.txt 中定义 `-DUSE_RGA=1`

## 🎨 UI 界面

基于 **EEZ Studio** + **LVGL 8** 设计：

| 页面 | 功能 |
|------|------|
| 主界面 | 时间、快捷入口、状态显示 |
| 音乐页面 | 播放控制、播放列表、歌词显示 |
| 天气页面 | 实时天气、预报、城市选择 |
| 传感器页面 | DHT11、MPU6050 数据显示 |
| 设置页面 | WiFi、亮度、音量调节 |
| CPU页面 | 三核CPU使用率实时监控 |
| 图片/视频页面 | 媒体浏览 |

## 🔧 配置说明

### WiFi 配置
WiFi 通过 `wpa_supplicant` 管理，支持：
- 扫描附近网络
- 密码输入连接
- 自动 DHCP 获取 IP
- 连接状态实时显示

**连接流程**：
1. 打开 WiFi 开关
2. 自动启动 wpa_supplicant（如未运行）
3. 扫描并显示附近网络列表
4. 点击网络输入密码
5. 自动连接并获取 IP

### MQTT 配置
在 `src/AppConfig.h` 中配置：
```cpp
#define APP_MQTT_HOST     "your_mqtt_broker"
#define APP_MQTT_PORT     1883
#define APP_MQTT_CLIENT_ID "rk3506_client"
```

**支持的远程命令**：
- LED 控制：`{"cmd":"on"}` / `{"cmd":"off"}`
- 蜂鸣器控制
- 传感器数据上报
- 红外遥控

### 天气 API 配置
在 `src/AppConfig.h` 中配置：
```cpp
#define APP_WEATHER_API_HOST "your_api_host"
#define APP_WEATHER_API_KEY  "your_api_key"
```

### 城市数据
城市列表使用 CSV 文件（`src/data/China-City-List-latest.csv`），运行时动态解析。

## 📦 依赖库

- **LVGL 8.x**: 图形界面库
- **libcurl**: HTTP 网络请求
- **wpa_supplicant**: WiFi 管理
- **FFmpeg**: 媒体解码
- **alsa-lib**: 音频播放
- **paho-mqtt**: MQTT 客户端
- **librga**: Rockchip RGA 硬件加速

## 📝 开发说明

### 添加新的 UI 动作
1. 在 `src/ui/actions.h` 声明函数
2. 在 `src/actions/eez_actions.cpp` 实现
3. 在 EEZ Studio 中绑定事件

### 添加后台服务
参考 `src/service/` 目录下的服务实现：
1. 继承单例模式
2. 实现 `init()` / `deinit()` 方法
3. 在 `main.cpp` 中初始化和绑定到指定 CPU

### 代码规范
- 使用 `.clang-format` 格式化代码
- 遵循 Doxygen 注释规范
- 中文使用 UTF-8 编码

## 🐛 调试

启用详细日志：
```cpp
// 在 main.cpp 中定义
#define DEBUG_LOG 1
```

查看 WiFi 调试信息：
```bash
# 在开发板上手动检查
wpa_cli scan
wpa_cli scan_results
ifconfig wlan0
```

## 📄 许可证

本项目作为 Luckfox RK3506 SDK 的一部分，遵循相应开源协议。

---
**开发板**: Luckfox Pico Ultra / RK3506  
**UI设计工具**: EEZ Studio  
**图形库**: LVGL 8.x  
**最后更新**: 2025-03
