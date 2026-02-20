/**
 * @file test_ir_cmd.cpp
 * @brief 红外命令管理交互式学习工具
 * @details 用于学习和管理红外设备命令，支持设备和命令命名
 */

#include <iostream>
#include <cstring>
#include <cstdio>
#include <atomic>
#include <iomanip>
#include <mutex>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include "../src/hal/IRDevice.h"
#include "../src/hal/IRCommandManager.h"

static bool g_running = true;

static uint8_t calcFrameChecksum(const std::vector<uint8_t> &frame, size_t start, size_t len)
{
    uint8_t sum = 0;
    const size_t end = std::min(frame.size(), start + len);
    for (size_t i = start; i < end; ++i)
    {
        sum += frame[i];
    }
    return sum;
}

static void printFrameDiagnostics(const std::vector<uint8_t> &frame)
{
    const bool hasMinSize = frame.size() >= 7;
    const bool headOk = hasMinSize && frame.front() == IR_FRAME_HEAD;
    const bool tailOk = hasMinSize && frame.back() == IR_FRAME_TAIL;

    uint16_t expectedLen = 0;
    bool lengthOk = false;
    bool checksumOk = false;
    uint8_t checksumCalc = 0;
    uint8_t checksumRecv = 0;
    uint8_t afn = 0;

    if (hasMinSize)
    {
        expectedLen = static_cast<uint16_t>(frame[1]) | (static_cast<uint16_t>(frame[2]) << 8);
        lengthOk = expectedLen == frame.size();
        afn = frame[4];

        if (lengthOk && expectedLen >= 7)
        {
            const size_t dataLen = expectedLen - 7;
            checksumCalc = calcFrameChecksum(frame, 3, 2 + dataLen);
            checksumRecv = frame[expectedLen - 2];
            checksumOk = checksumCalc == checksumRecv;
        }
    }

    std::cout << "[协议诊断] 帧长=" << frame.size()
              << ", 头=" << (headOk ? "OK" : "BAD")
              << ", 尾=" << (tailOk ? "OK" : "BAD")
              << ", 长度字段=" << expectedLen
              << ", 长度一致=" << (lengthOk ? "YES" : "NO")
              << ", AFN=0x" << std::hex << std::setw(2) << std::setfill('0') << (int)afn
              << std::dec
              << ", 校验=" << (checksumOk ? "OK" : "BAD")
              << "(calc=0x" << std::hex << std::setw(2) << std::setfill('0') << (int)checksumCalc
              << ", recv=0x" << std::setw(2) << (int)checksumRecv << ")"
              << std::dec << std::endl;
}

void signalHandler(int sig)
{
    std::cout << "\n收到信号 " << sig << "，正在退出..." << std::endl;
    g_running = false;
}

void printHelp()
{
    std::cout << "\n命令列表:" << std::endl;
    std::cout << "  list                          - 列出所有设备" << std::endl;
    std::cout << "  list <device>                 - 列出指定设备的所有命令" << std::endl;
    std::cout << "  learn <device> <command>      - 外部学习并保存命令" << std::endl;
    std::cout << "  emit <device> <command>       - 发射命令" << std::endl;
    std::cout << "  remove <device> <command>     - 删除命令" << std::endl;
    std::cout << "  removeidx <device> <index>    - 按序号删除命令" << std::endl;
    std::cout << "  rmdevice <device>             - 删除整个设备" << std::endl;
    std::cout << "  debug on/off                  - 协议抓包日志开关" << std::endl;
    std::cout << "  q/quit                        - 退出" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    const char *devPath = "/dev/ttyS1";
    if (argc >= 2)
    {
        devPath = argv[1];
    }

    std::cout << "=== 红外命令管理学习工具 ===" << std::endl;
    std::cout << "设备: " << devPath << std::endl;
    std::cout << std::endl;

    // 1. 初始化红外设备
    std::cout << "[1/3] 初始化红外设备..." << std::endl;
    IRDevice &ir = IRDevice::getInstance();
    if (!ir.open())
    {
        std::cerr << "错误: 无法打开红外设备 " << devPath << std::endl;
        return -1;
    }
    std::cout << "   成功!" << std::endl;

    // 2. 初始化命令管理器
    std::cout << "[2/3] 初始化命令管理器..." << std::endl;
    IRCommandManager &mgr = IRCommandManager::getInstance();
    std::string commandStorePath = "/data/ir_commands";
    if (!mgr.init(commandStorePath))
    {
        commandStorePath = "/root/ir_commands";
        std::cerr << "警告: /data/ir_commands 不可用，回退到 " << commandStorePath << std::endl;

        if (!mgr.init(commandStorePath))
        {
            std::cerr << "错误: 命令管理器初始化失败，无法保存学习结果" << std::endl;
            return -1;
        }
    }
    std::cout << "   成功! 存储路径: " << commandStorePath << std::endl;

    // 3. 设置回调
    std::cout << "[3/3] 设置回调..." << std::endl;
    std::atomic<bool> learnComplete{false};
    std::atomic<bool> protocolDebug{false};
    std::string learnDevice;
    std::string learnCommand;
    std::vector<uint8_t> learnedData;
    std::mutex learnedDataMutex;

    ir.setOnStatusChange([](IRLearnStatus status) {
        const char *statusStr[] = {"空闲", "学习中", "成功", "超时", "错误"};
        if (status >= IRLearnStatus::IDLE && status <= IRLearnStatus::ERROR)
        {
            std::cout << "\n[状态] " << statusStr[static_cast<int>(status)] << std::endl;
        }
    });

    ir.setOnExtLearnComplete([&](const std::vector<uint8_t> &data) {
        {
            std::lock_guard<std::mutex> lock(learnedDataMutex);
            learnedData = data;
        }
        learnComplete.store(true, std::memory_order_release);
        std::cout << "\n[学习完成] 数据长度: " << data.size() << " 字节" << std::endl;
        
        std::cout << "数据: ";
        for (size_t i = 0; i < data.size() && i < 32; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
        }
        std::cout << std::dec;
        if (data.size() > 32) std::cout << "...";
        std::cout << std::endl;

        if (protocolDebug.load(std::memory_order_acquire))
        {
            printFrameDiagnostics(data);
        }
    });
    std::cout << "   成功!" << std::endl;

    // 显示当前设备
    std::cout << std::endl;
    auto devices = mgr.getDeviceList();
    if (!devices.empty())
    {
        std::cout << "当前已保存 " << devices.size() << " 个设备:" << std::endl;
        for (auto &dev : devices)
        {
            auto cmds = mgr.getCommandList(dev);
            std::cout << "  - " << dev << " (" << cmds.size() << " 个命令)" << std::endl;
        }
    }
    else
    {
        std::cout << "暂无保存的设备，使用 'learn' 命令开始学习!" << std::endl;
    }

    printHelp();

    // 主循环
    char cmd[128];
    while (g_running)
    {
        std::cout << "\nir_cmd> ";
        std::cin.getline(cmd, sizeof(cmd));

        if (strlen(cmd) == 0)
            continue;

        if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0)
        {
            break;
        }
        else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0)
        {
            printHelp();
        }
        else if (strcmp(cmd, "list") == 0)
        {
            // 列出所有设备
            auto devList = mgr.getDeviceList();
            if (devList.empty())
            {
                std::cout << "暂无设备" << std::endl;
            }
            else
            {
                std::cout << "设备列表 (" << devList.size() << "):" << std::endl;
                for (auto &dev : devList)
                {
                    auto cmdList = mgr.getCommandList(dev);
                    std::cout << "  [" << dev << "] - " << cmdList.size() << " 个命令" << std::endl;
                }
            }
        }
        else if (strncmp(cmd, "list ", 5) == 0)
        {
            // 列出指定设备的命令
            std::string deviceName = cmd + 5;
            auto cmdList = mgr.getCommandList(deviceName);
            if (cmdList.empty())
            {
                std::cout << "设备 '" << deviceName << "' 不存在或无命令" << std::endl;
            }
            else
            {
                std::cout << "设备 '" << deviceName << "' 的命令 (" << cmdList.size() << "):" << std::endl;
                for (size_t i = 0; i < cmdList.size(); ++i)
                {
                    std::cout << "  [" << i << "] " << cmdList[i] << std::endl;
                }
            }
        }
        else if (strncmp(cmd, "learn ", 6) == 0)
        {
            // 学习命令
            char deviceName[64];
            char commandName[64];
            if (sscanf(cmd + 6, "%s %s", deviceName, commandName) == 2)
            {
                if (mgr.commandExists(deviceName, commandName))
                {
                    std::cout << "警告: 命令已存在，将覆盖!" << std::endl;
                }

                learnComplete.store(false, std::memory_order_release);
                {
                    std::lock_guard<std::mutex> lock(learnedDataMutex);
                    learnedData.clear();
                }
                learnDevice = deviceName;
                learnCommand = commandName;

                std::cout << "开始学习: [" << deviceName << "] - " << commandName << std::endl;
                std::cout << "请将遥控器对准红外模块，按下要学习的按钮..." << std::endl;

                if (!ir.startExtLearn())
                {
                    std::cerr << "错误: 开始外部学习失败" << std::endl;
                    continue;
                }

                // 等待学习完成
                int timeout = 30;
                while (timeout-- > 0 && g_running && !learnComplete.load(std::memory_order_acquire))
                {
                    sleep(1);
                }

                std::vector<uint8_t> learnedDataCopy;
                {
                    std::lock_guard<std::mutex> lock(learnedDataMutex);
                    learnedDataCopy = learnedData;
                }

                if (learnComplete.load(std::memory_order_acquire) && !learnedDataCopy.empty())
                {
                    if (mgr.addCommand(deviceName, commandName, learnedDataCopy))
                    {
                        std::cout << "保存成功!" << std::endl;
                    }
                    else
                    {
                        std::cerr << "错误: 保存失败" << std::endl;
                    }
                }
                else
                {
                    std::cerr << "学习超时或失败!" << std::endl;
                    ir.stopExtLearn();
                }
            }
            else
            {
                std::cout << "用法: learn <设备名> <命令名>" << std::endl;
                std::cout << "示例: learn 空调 开关" << std::endl;
            }
        }
        else if (strcmp(cmd, "debug on") == 0)
        {
            protocolDebug.store(true, std::memory_order_release);
            std::cout << "协议抓包日志: 已开启" << std::endl;
        }
        else if (strcmp(cmd, "debug off") == 0)
        {
            protocolDebug.store(false, std::memory_order_release);
            std::cout << "协议抓包日志: 已关闭" << std::endl;
        }
        else if (strncmp(cmd, "emit ", 5) == 0)
        {
            // 发射命令
            char deviceName[64];
            char commandName[64];
            if (sscanf(cmd + 5, "%s %s", deviceName, commandName) == 2)
            {
                if (!mgr.commandExists(deviceName, commandName))
                {
                    std::cout << "错误: 命令不存在" << std::endl;
                    continue;
                }

                std::cout << "发射: [" << deviceName << "] - " << commandName << std::endl;
                if (mgr.emitCommand(deviceName, commandName))
                {
                    std::cout << "发射成功!" << std::endl;
                }
                else
                {
                    std::cerr << "错误: 发射失败" << std::endl;
                }
            }
            else
            {
                std::cout << "用法: emit <设备名> <命令名>" << std::endl;
            }
        }
        else if (strncmp(cmd, "remove ", 7) == 0)
        {
            // 删除命令
            char deviceName[64];
            char commandName[64];
            if (sscanf(cmd + 7, "%s %s", deviceName, commandName) == 2)
            {
                if (!mgr.commandExists(deviceName, commandName))
                {
                    std::cout << "错误: 命令不存在" << std::endl;
                    continue;
                }

                std::cout << "删除: [" << deviceName << "] - " << commandName << std::endl;
                if (mgr.removeCommand(deviceName, commandName))
                {
                    std::cout << "删除成功!" << std::endl;
                }
                else
                {
                    std::cerr << "错误: 删除失败" << std::endl;
                }
            }
            else
            {
                std::cout << "用法: remove <设备名> <命令名>" << std::endl;
            }
        }
        else if (strncmp(cmd, "removeidx ", 10) == 0)
        {
            char deviceName[64];
            int index = -1;
            if (sscanf(cmd + 10, "%63s %d", deviceName, &index) == 2)
            {
                auto cmdList = mgr.getCommandList(deviceName);
                if (cmdList.empty())
                {
                    std::cout << "错误: 设备不存在或无命令" << std::endl;
                    continue;
                }

                if (index < 0 || static_cast<size_t>(index) >= cmdList.size())
                {
                    std::cout << "错误: 序号超出范围，有效范围 0-" << (cmdList.size() - 1) << std::endl;
                    continue;
                }

                const std::string &targetCmd = cmdList[static_cast<size_t>(index)];
                std::cout << "删除: [" << deviceName << "] - " << targetCmd
                          << " (index=" << index << ")" << std::endl;
                if (mgr.removeCommand(deviceName, targetCmd))
                {
                    std::cout << "删除成功!" << std::endl;
                }
                else
                {
                    std::cerr << "错误: 删除失败" << std::endl;
                }
            }
            else
            {
                std::cout << "用法: removeidx <设备名> <序号>" << std::endl;
                std::cout << "示例: removeidx 空调 1" << std::endl;
            }
        }
        else if (strncmp(cmd, "rmdevice ", 9) == 0)
        {
            // 删除整个设备
            std::string deviceName = cmd + 9;
            std::cout << "删除设备: " << deviceName << std::endl;
            if (mgr.removeDevice(deviceName))
            {
                std::cout << "删除成功!" << std::endl;
            }
            else
            {
                std::cerr << "错误: 删除失败或设备不存在" << std::endl;
            }
        }
        else
        {
            std::cout << "未知命令，输入 help 查看帮助" << std::endl;
        }
    }

    mgr.deinit();
    ir.close();
    std::cout << "退出" << std::endl;
    return 0;
}
