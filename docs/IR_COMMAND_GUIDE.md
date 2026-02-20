# 红外命令管理使用指南

## 概述

红外命令管理系统允许你按 **设备名 + 命令名** 的方式组织和管理红外码，支持外部学习、保存和发射。

## 文件说明

| 文件 | 说明 |
|------|------|
| `src/hal/IRCommandManager.h` | 红外命令管理器头文件 |
| `src/hal/IRCommandManager.cpp` | 红外命令管理器实现 |
| `tests/test_ir_cmd.cpp` | 交互式学习工具 |
| `examples/example_ir_usage.c` | C接口使用示例 |

## 数据存储

红外码数据保存在：`/data/ir_commands/ir_commands.db`

## 快速开始

### 1. 使用交互式工具学习命令

在RK3506设备上运行：

```bash
./test_ir_cmd /dev/ttyS1
```

### 2. 学习空调命令示例

```
ir_cmd> learn 空调 开关
开始学习: [空调] - 开关
请将遥控器对准红外模块，按下要学习的按钮...

[状态] 学习中

[学习完成] 数据长度: 128 字节
数据: 01 02 03 04 ...
保存成功!
```

继续学习其他命令：

```
ir_cmd> learn 空调 制冷
ir_cmd> learn 空调 制热
ir_cmd> learn 空调 温度+
ir_cmd> learn 空调 温度-
```

### 3. 学习电视命令示例

```
ir_cmd> learn 电视 开关
ir_cmd> learn 电视 音量+
ir_cmd> learn 电视 音量-
ir_cmd> learn 电视 频道+
ir_cmd> learn 电视 频道-
```

### 4. 查看已保存的设备和命令

```
ir_cmd> list
设备列表 (2):
  [空调] - 5 个命令
  [电视] - 5 个命令

ir_cmd> list 空调
设备 '空调' 的命令 (5):
  - 开关
  - 制冷
  - 制热
  - 温度+
  - 温度-
```

### 5. 发射命令

```
ir_cmd> emit 空调 开关
发射: [空调] - 开关
发射成功!

ir_cmd> emit 电视 音量+
发射: [电视] - 音量+
发射成功!
```

## AppBridge C 接口调用

### 初始化

```c
#include "AppBridge.h"

// 初始化桥接层
bridge_init();

// 初始化红外命令管理器
bridge_ir_cmd_init("/data/ir_commands");
```

### 外部学习并保存

```c
void learnCallback(int success, const char *deviceName, const char *commandName)
{
    if (success) {
        printf("学习成功: %s - %s\n", deviceName, commandName);
    } else {
        printf("学习失败\n");
    }
}

// 学习空调"开关"命令
bridge_ir_ext_learn_and_save("空调", "开关", learnCallback);
```

### 发射命令

```c
// 发射空调"开关"
if (bridge_ir_cmd_emit("空调", "开关") == 0) {
    printf("发射成功\n");
}

// 发射电视"音量+"
bridge_ir_cmd_emit("电视", "音量+");
```

### 获取设备列表

```c
size_t count = 0;
char **devices = bridge_ir_cmd_get_devices(&count);

if (devices) {
    for (size_t i = 0; i < count; i++) {
        printf("- %s\n", devices[i]);
    }
    bridge_ir_cmd_free_devices(devices, count);
}
```

### 获取指定设备的命令列表

```c
size_t count = 0;
char **commands = bridge_ir_cmd_get_commands("空调", &count);

if (commands) {
    for (size_t i = 0; i < count; i++) {
        printf("- %s\n", commands[i]);
    }
    bridge_ir_cmd_free_commands(commands, count);
}
```

### 检查命令是否存在

```c
if (bridge_ir_cmd_exists("空调", "开关")) {
    printf("命令存在\n");
}
```

### 删除命令

```c
// 删除单个命令
bridge_ir_cmd_remove("空调", "开关");

// 删除整个设备（包括所有命令）
bridge_ir_cmd_remove_device("旧设备");
```

### 清理

```c
bridge_ir_cmd_deinit();
bridge_deinit();
```

## 完整示例

参考：[examples/example_ir_usage.c](../examples/example_ir_usage.c)

## 推荐的设备和命令命名规范

| 设备类型 | 设备名示例 | 命令名示例 |
|---------|-----------|-----------|
| 空调 | 空调、客厅空调、卧室空调 | 开关、制冷、制热、除湿、送风、温度+、温度-、模式 |
| 电视 | 电视、客厅电视 | 开关、音量+、音量-、静音、频道+、频道-、源切换 |
| 机顶盒 | 机顶盒 | 开关、音量+、音量-、频道+、频道-、确认、返回、上下左右 |
| 风扇 | 风扇 | 开关、风速+、风速-、摇头、定时 |
| 灯具 | 灯具、客厅灯 | 开关、亮度+、亮度-、色温 |

## Home Assistant 集成示例

在Home Assistant中可以通过MQTT调用：

```yaml
# configuration.yaml
mqtt:
  switch:
    - name: "客厅空调开关"
      command_topic: "home/ir/command"
      payload_on: '{"device":"空调","command":"开关"}'
      payload_off: '{"device":"空调","command":"开关"}'

    - name: "客厅空调制冷"
      command_topic: "home/ir/command"
      payload_on: '{"device":"空调","command":"制冷"}'
```
