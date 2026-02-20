/**
 * @file test_ir.cpp
 * @brief 红外学习模块测试程序
 * @details 用于测试红外学习模块的学习、发射等功能
 */

#include <iostream>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <unistd.h>
#include <signal.h>
#include "../src/hal/IRDevice.h"

static bool g_running = true;
static IRDevice *g_irDevice = nullptr;

/**
 * @brief 信号处理函数
 */
void signalHandler(int sig)
{
    std::cout << "\n收到信号 " << sig << "，正在退出..." << std::endl;
    g_running = false;
}

/**
 * @brief 打印使用说明
 */
void printUsage(const char *prog)
{
    std::cout << "用法: " << prog << " <串口设备路径> [命令] [参数]" << std::endl;
    std::cout << std::endl;
    std::cout << "命令:" << std::endl;
    std::cout << "  learn <index>     - 学习红外码到指定索引 (0-255)" << std::endl;
    std::cout << "  extlearn          - 外部学习模式（数据直接返回给RK3506）" << std::endl;
    std::cout << "  emit <index>      - 发射指定索引的红外码" << std::endl;
    std::cout << "  emitraw <file>    - 发射原始红外码数据文件" << std::endl;
    std::cout << "  clear <index>     - 清除指定索引的红外码" << std::endl;
    std::cout << "  clearall          - 清除所有红外码" << std::endl;
    std::cout << "  interactive       - 交互式测试模式" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << prog << " /dev/ttyS1 learn 0       # 学习红外码到索引0" << std::endl;
    std::cout << "  " << prog << " /dev/ttyS1 emit 0        # 发射索引0的红外码" << std::endl;
    std::cout << "  " << prog << " /dev/ttyS1 interactive   # 进入交互式模式" << std::endl;
}

/**
 * @brief 学习红外码
 */
int cmdLearn(const char *devicePath, uint8_t index)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "开始学习索引 " << (int)index << "..." << std::endl;
    std::cout << "请将遥控器对准红外模块，按下要学习的按钮..." << std::endl;

    // 设置状态回调
    ir.setOnStatusChange([](IRLearnStatus status) {
        const char *statusStr[] = {"空闲", "学习中", "成功", "超时", "错误"};
        if (status >= IRLearnStatus::IDLE && status <= IRLearnStatus::ERROR)
        {
            std::cout << "[状态] " << statusStr[static_cast<int>(status)] << std::endl;
        }
    });

    // 设置完成回调
    bool learnComplete = false;
    IRCode learnedCode;
    ir.setOnLearnComplete([&learnComplete, &learnedCode](const IRCode &code) {
        learnComplete = true;
        learnedCode = code;
        std::cout << "[完成] 学习成功！数据长度: " << code.data.size() << " 字节" << std::endl;
    });

    // 开始学习
    if (!ir.startLearn(index))
    {
        std::cerr << "错误: 开始学习失败" << std::endl;
        return -1;
    }

    // 等待学习完成（最多30秒）
    int timeout = 30;
    while (timeout-- > 0 && g_running)
    {
        sleep(1);

        IRLearnStatus status = ir.getLearnStatus();
        if (status == IRLearnStatus::SUCCESS)
        {
            std::cout << "学习成功！" << std::endl;
            return 0;
        }
        else if (status == IRLearnStatus::ERROR)
        {
            std::cerr << "学习失败！" << std::endl;
            return -1;
        }

        std::cout << "等待中... " << timeout << " 秒" << std::endl;
    }

    if (!g_running)
    {
        std::cout << "用户中断" << std::endl;
        return -1;
    }

    std::cerr << "学习超时！" << std::endl;
    ir.stopLearn();
    return -1;
}

/**
 * @brief 外部学习红外码（数据返回给主控）
 */
int cmdExtLearn(const char *devicePath)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "开始外部学习模式，请按下遥控器按钮..." << std::endl;

    // 设置状态回调
    ir.setOnStatusChange([](IRLearnStatus status) {
        const char *statusStr[] = {"空闲", "学习中", "成功", "超时", "错误"};
        if (status >= IRLearnStatus::IDLE && status <= IRLearnStatus::ERROR)
        {
            std::cout << "[状态] " << statusStr[static_cast<int>(status)] << std::endl;
        }
    });

    // 设置外部学习完成回调
    bool extLearnComplete = false;
    std::vector<uint8_t> learnedRawCode;
    ir.setOnExtLearnComplete([&extLearnComplete, &learnedRawCode](const std::vector<uint8_t> &code) {
        extLearnComplete = true;
        learnedRawCode = code;
        std::cout << "[外部学习完成] 原始数据长度: " << code.size() << " 字节" << std::endl;
        
        // 打印前16个字节
        std::cout << "数据: ";
        for (size_t i = 0; i < code.size() && i < 32; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)code[i] << " ";
        }
        std::cout << std::dec;
        if (code.size() > 32) std::cout << "...";
        std::cout << std::endl;

        // 保存到文件
        FILE *f = fopen("ir_code_raw.bin", "wb");
        if (f)
        {
            fwrite(code.data(), 1, code.size(), f);
            fclose(f);
            std::cout << "已保存到: ir_code_raw.bin" << std::endl;
        }
    });

    // 开始外部学习
    if (!ir.startExtLearn())
    {
        std::cerr << "错误: 开始外部学习失败" << std::endl;
        return -1;
    }

    // 等待学习完成（最多30秒）
    int timeout = 30;
    while (timeout-- > 0 && g_running)
    {
        sleep(1);
        if (extLearnComplete)
        {
            std::cout << "外部学习成功！" << std::endl;
            return 0;
        }
        std::cout << "等待中... " << timeout << " 秒" << std::endl;
    }

    if (!g_running)
    {
        std::cout << "用户中断" << std::endl;
        return -1;
    }

    std::cerr << "外部学习超时！" << std::endl;
    ir.stopExtLearn();
    return -1;
}

/**
 * @brief 发射红外码
 */
int cmdEmit(const char *devicePath, uint8_t index)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "发射索引 " << (int)index << " 的红外码..." << std::endl;

    if (ir.emitCode(index))
    {
        std::cout << "发射成功！" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "发射失败！" << std::endl;
        return -1;
    }
}

/**
 * @brief 清除红外码
 */
int cmdClear(const char *devicePath, uint8_t index)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "清除索引 " << (int)index << " 的红外码..." << std::endl;

    if (ir.clearCode(index))
    {
        std::cout << "清除成功！" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "清除失败！" << std::endl;
        return -1;
    }
}

/**
 * @brief 清除所有红外码
 */
int cmdClearAll(const char *devicePath)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "清除所有红外码..." << std::endl;

    if (ir.clearAllCodes())
    {
        std::cout << "清除成功！" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "清除失败！" << std::endl;
        return -1;
    }
}

/**
 * @brief 交互式模式
 */
int cmdInteractive(const char *devicePath)
{
    IRDevice ir(devicePath);

    if (!ir.open())
    {
        std::cerr << "错误: 无法打开设备 " << devicePath << std::endl;
        return -1;
    }

    std::cout << "=== 红外学习模块交互式测试 ===" << std::endl;
    std::cout << "设备: " << devicePath << std::endl;
    std::cout << std::endl;
    std::cout << "命令:" << std::endl;
    std::cout << "  l <index>  - 学习红外码（内部存储）" << std::endl;
    std::cout << "  x          - 外部学习模式（数据存到RK3506）" << std::endl;
    std::cout << "  e <index>  - 发射红外码" << std::endl;
    std::cout << "  c <index>  - 清除红外码" << std::endl;
    std::cout << "  a          - 清除所有红外码" << std::endl;
    std::cout << "  q          - 退出" << std::endl;
    std::cout << std::endl;

    // 设置回调
    ir.setOnStatusChange([](IRLearnStatus status) {
        const char *statusStr[] = {"空闲", "学习中", "成功", "超时", "错误"};
        if (status >= IRLearnStatus::IDLE && status <= IRLearnStatus::ERROR)
        {
            std::cout << "\n[状态变化] " << statusStr[static_cast<int>(status)] << std::endl;
        }
    });

    ir.setOnLearnComplete([](const IRCode &code) {
        std::cout << "\n[学习完成] 索引: " << (int)code.index
                  << ", 数据长度: " << code.data.size() << " 字节" << std::endl;
    });

    ir.setOnExtLearnComplete([](const std::vector<uint8_t> &code) {
        std::cout << "\n[外部学习完成] 原始数据长度: " << code.size() << " 字节" << std::endl;
        
        std::cout << "数据: ";
        for (size_t i = 0; i < code.size() && i < 32; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)code[i] << " ";
        }
        std::cout << std::dec;
        if (code.size() > 32) std::cout << "...";
        std::cout << std::endl;

        FILE *f = fopen("ir_code_raw.bin", "wb");
        if (f)
        {
            fwrite(code.data(), 1, code.size(), f);
            fclose(f);
            std::cout << "已保存到: ir_code_raw.bin" << std::endl;
        }
    });

    char cmd[32];
    int index;

    while (g_running)
    {
        std::cout << "\nir> ";
        std::cin >> cmd;

        if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0)
        {
            break;
        }
        else if (strcmp(cmd, "l") == 0 || strcmp(cmd, "learn") == 0)
        {
            std::cin >> index;
            if (index < 0 || index > 255)
            {
                std::cerr << "错误: 索引必须在 0-255 之间" << std::endl;
                continue;
            }

            std::cout << "开始学习索引 " << index << "，请按下遥控器按钮..." << std::endl;

            if (!ir.startLearn(static_cast<uint8_t>(index)))
            {
                std::cerr << "开始学习失败！" << std::endl;
                continue;
            }

            // 等待学习完成
            int timeout = 30;
            while (timeout-- > 0 && g_running)
            {
                sleep(1);
                IRLearnStatus status = ir.getLearnStatus();
                if (status == IRLearnStatus::SUCCESS)
                {
                    std::cout << "学习成功！" << std::endl;
                    break;
                }
                else if (status == IRLearnStatus::ERROR)
                {
                    std::cerr << "学习失败！" << std::endl;
                    break;
                }
            }

            if (timeout < 0)
            {
                std::cerr << "学习超时！" << std::endl;
                ir.stopLearn();
            }
        }
        else if (strcmp(cmd, "x") == 0 || strcmp(cmd, "extlearn") == 0)
        {
            std::cout << "开始外部学习模式，请按下遥控器按钮..." << std::endl;

            if (!ir.startExtLearn())
            {
                std::cerr << "开始外部学习失败！" << std::endl;
                continue;
            }

            // 等待学习完成
            int timeout = 30;
            while (timeout-- > 0 && g_running)
            {
                sleep(1);
                IRLearnStatus status = ir.getLearnStatus();
                if (status == IRLearnStatus::SUCCESS)
                {
                    std::cout << "外部学习成功！" << std::endl;
                    break;
                }
                else if (status == IRLearnStatus::ERROR)
                {
                    std::cerr << "外部学习失败！" << std::endl;
                    break;
                }
            }

            if (timeout < 0)
            {
                std::cerr << "外部学习超时！" << std::endl;
                ir.stopExtLearn();
            }
        }
        else if (strcmp(cmd, "e") == 0 || strcmp(cmd, "emit") == 0)
        {
            std::cin >> index;
            if (index < 0 || index > 255)
            {
                std::cerr << "错误: 索引必须在 0-255 之间" << std::endl;
                continue;
            }

            std::cout << "发射索引 " << index << " 的红外码..." << std::endl;
            if (ir.emitCode(static_cast<uint8_t>(index)))
            {
                std::cout << "发射成功！" << std::endl;
            }
            else
            {
                std::cerr << "发射失败！" << std::endl;
            }
        }
        else if (strcmp(cmd, "c") == 0 || strcmp(cmd, "clear") == 0)
        {
            std::cin >> index;
            if (index < 0 || index > 255)
            {
                std::cerr << "错误: 索引必须在 0-255 之间" << std::endl;
                continue;
            }

            std::cout << "清除索引 " << index << " 的红外码..." << std::endl;
            if (ir.clearCode(static_cast<uint8_t>(index)))
            {
                std::cout << "清除成功！" << std::endl;
            }
            else
            {
                std::cerr << "清除失败！" << std::endl;
            }
        }
        else if (strcmp(cmd, "a") == 0 || strcmp(cmd, "clearall") == 0)
        {
            std::cout << "清除所有红外码..." << std::endl;
            if (ir.clearAllCodes())
            {
                std::cout << "清除成功！" << std::endl;
            }
            else
            {
                std::cerr << "清除失败！" << std::endl;
            }
        }
        else if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0)
        {
            std::cout << "命令:" << std::endl;
            std::cout << "  l <index>  - 学习红外码（内部存储）" << std::endl;
            std::cout << "  x          - 外部学习模式（数据存到RK3506）" << std::endl;
            std::cout << "  e <index>  - 发射红外码" << std::endl;
            std::cout << "  c <index>  - 清除红外码" << std::endl;
            std::cout << "  a          - 清除所有红外码" << std::endl;
            std::cout << "  q          - 退出" << std::endl;
        }
        else
        {
            std::cerr << "未知命令: " << cmd << "，输入 h 查看帮助" << std::endl;
        }
    }

    std::cout << "退出交互式模式" << std::endl;
    return 0;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    // 设置信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    const char *devicePath = argv[1];

    // 如果只提供设备路径，进入交互式模式
    if (argc == 2)
    {
        return cmdInteractive(devicePath);
    }

    const char *cmd = argv[2];

    if (strcmp(cmd, "learn") == 0 && argc >= 4)
    {
        return cmdLearn(devicePath, static_cast<uint8_t>(atoi(argv[3])));
    }
    else if (strcmp(cmd, "extlearn") == 0)
    {
        return cmdExtLearn(devicePath);
    }
    else if (strcmp(cmd, "emit") == 0 && argc >= 4)
    {
        return cmdEmit(devicePath, static_cast<uint8_t>(atoi(argv[3])));
    }
    else if (strcmp(cmd, "clear") == 0 && argc >= 4)
    {
        return cmdClear(devicePath, static_cast<uint8_t>(atoi(argv[3])));
    }
    else if (strcmp(cmd, "clearall") == 0)
    {
        return cmdClearAll(devicePath);
    }
    else if (strcmp(cmd, "interactive") == 0)
    {
        return cmdInteractive(devicePath);
    }
    else
    {
        printUsage(argv[0]);
        return 1;
    }
}
