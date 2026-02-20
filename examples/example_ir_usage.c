/**
 * @file example_ir_usage.c
 * @brief 红外命令管理使用示例
 * @details 演示如何使用AppBridge中的红外命令管理接口
 */

#include <stdio.h>
#include <unistd.h>
#include "../src/bridge/AppBridge.h"

// 外部学习完成回调
void extLearnCompleteCallback(int success, const char *deviceName, const char *commandName)
{
    printf("[回调] 外部学习完成!\n");
    printf("  成功: %s\n", success ? "是" : "否");
    printf("  设备: %s\n", deviceName);
    printf("  命令: %s\n", commandName);
}

int main(int argc, char *argv[])
{
    printf("=== 红外命令管理使用示例 ===\n\n");

    // 1. 初始化桥接层
    printf("1. 初始化桥接层...\n");
    if (bridge_init() != 0)
    {
        printf("   错误: 桥接层初始化失败\n");
        return -1;
    }
    printf("   成功!\n\n");

    // 2. 初始化红外命令管理器
    printf("2. 初始化红外命令管理器...\n");
    if (bridge_ir_cmd_init("/data/ir_commands") != 0)
    {
        printf("   错误: 红外命令管理器初始化失败\n");
        bridge_deinit();
        return -1;
    }
    printf("   成功!\n\n");

    // 3. 获取设备列表
    printf("3. 获取设备列表...\n");
    size_t devCount = 0;
    char **devices = bridge_ir_cmd_get_devices(&devCount);
    if (devices && devCount > 0)
    {
        printf("   已找到 %zu 个设备:\n", devCount);
        for (size_t i = 0; i < devCount; i++)
        {
            printf("   - %s\n", devices[i]);
        }
    }
    else
    {
        printf("   暂无设备\n");
    }
    bridge_ir_cmd_free_devices(devices, devCount);
    printf("\n");

    // 4. 示例: 添加一个空调命令 (实际使用时数据通过外部学习获取)
    printf("4. 示例: 添加空调命令...\n");
    uint8_t dummyData[] = {0x01, 0x02, 0x03, 0x04}; // 示例数据
    if (bridge_ir_cmd_add("空调", "开关", dummyData, sizeof(dummyData)) == 0)
    {
        printf("   添加成功!\n");
    }
    printf("\n");

    // 5. 检查命令是否存在
    printf("5. 检查命令是否存在...\n");
    if (bridge_ir_cmd_exists("空调", "开关"))
    {
        printf("   命令存在\n");
    }
    else
    {
        printf("   命令不存在\n");
    }
    printf("\n");

    // 6. 获取空调的所有命令
    printf("6. 获取空调的所有命令...\n");
    size_t cmdCount = 0;
    char **commands = bridge_ir_cmd_get_commands("空调", &cmdCount);
    if (commands && cmdCount > 0)
    {
        printf("   空调共有 %zu 个命令:\n", cmdCount);
        for (size_t i = 0; i < cmdCount; i++)
        {
            printf("   - %s\n", commands[i]);
        }
    }
    bridge_ir_cmd_free_commands(commands, cmdCount);
    printf("\n");

    // 7. 发射命令 (实际使用时取消注释)
    printf("7. 发射命令...\n");
    // if (bridge_ir_cmd_emit("空调", "开关") == 0)
    // {
    //     printf("   发射成功!\n");
    // }
    printf("   (已注释，实际使用时取消注释)\n\n");

    // 8. 外部学习并保存 (实际使用时取消注释)
    printf("8. 外部学习并保存...\n");
    // printf("   请按下遥控器按钮...\n");
    // if (bridge_ir_ext_learn_and_save("空调", "制冷", extLearnCompleteCallback) == 0)
    // {
    //     printf("   外部学习已启动，请等待...\n");
    //     sleep(30); // 等待学习完成
    // }
    printf("   (已注释，实际使用时取消注释)\n\n");

    // 9. 清理
    printf("9. 清理...\n");
    bridge_ir_cmd_deinit();
    bridge_deinit();
    printf("   完成!\n");

    printf("\n=== 示例结束 ===\n");
    return 0;
}
