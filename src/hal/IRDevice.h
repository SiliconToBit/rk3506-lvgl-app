/**
 * @file IRDevice.h
 * @brief 红外学习模块驱动类
 * @details 通过串口与红外学习模块通信，支持红外码学习和发射功能
 *          协议格式：帧头(68H) + 长度 + 地址 + 命令 + 数据 + 校验 + 帧尾(16H)
 */

#ifndef LVGL_APP_HAL_IRDEVICE_H
#define LVGL_APP_HAL_IRDEVICE_H

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

// 帧定义
constexpr uint8_t IR_FRAME_HEAD = 0x68;
constexpr uint8_t IR_FRAME_TAIL = 0x16;

// 地址
constexpr uint8_t IR_ADDR_LOCAL = 0x00;       // 本机地址
constexpr uint8_t IR_ADDR_BROADCAST = 0xFF;   // 广播地址

// 命令码 (内部存储)
constexpr uint8_t IR_CMD_LEARN = 0x10;       // 进入内部编码学习模式
constexpr uint8_t IR_CMD_STOP_LEARN = 0x11;  // 退出内部编码学习模式
constexpr uint8_t IR_CMD_EMIT = 0x12;        // 发送内部存储编码
constexpr uint8_t IR_CMD_READ = 0x18;         // 读取内部存储编码

// 命令码 (外部存储)
constexpr uint8_t IR_CMD_EXT_LEARN = 0x20;    // 进入外部编码学习模式
constexpr uint8_t IR_CMD_EXT_STOP_LEARN = 0x21; // 退出外部编码学习模式
constexpr uint8_t IR_CMD_EXT_EMIT = 0x22;     // 发送外部存储编码

// 系统命令
constexpr uint8_t IR_CMD_FORMAT = 0x08;       // 格式化

// 内部存储清除 - 说明书未单独定义清除单个的AFN，我们保留定义用于兼容性
constexpr uint8_t IR_CMD_CLEAR = 0x13;        // 清除单个（备用）
constexpr uint8_t IR_CMD_CLEAR_ALL = 0x14;    // 清除所有（备用）

// 应答码
constexpr uint8_t IR_ACK_SUCCESS = 0x01;     // 成功
constexpr uint8_t IR_ACK_FAIL = 0x02;        // 失败
constexpr uint8_t IR_ACK_LEARN_COMPLETE = 0x02; // 学习完成（上传）

// 最大红外码长度
constexpr size_t IR_MAX_CODE_LENGTH = 1024;

/**
 * @brief 红外学习模块状态
 */
enum class IRLearnStatus
{
    IDLE,           // 空闲
    LEARNING,       // 学习中
    SUCCESS,        // 学习成功
    TIMEOUT,        // 超时
    ERROR           // 错误
};

/**
 * @brief 红外码数据结构
 */
struct IRCode
{
    uint8_t index;                          // 存储索引
    std::vector<uint8_t> data;              // 红外码数据
    uint32_t timestamp;                     // 学习时间戳

    IRCode() : index(0), timestamp(0) {}
};

class IRDevice
{
  private:
    std::string m_devPath;
    int m_fd;
    bool m_isLearning;
    IRLearnStatus m_learnStatus;
    IRCode m_lastLearnedCode;

    std::function<void(const IRCode&)> m_onLearnComplete;  // 学习完成回调
    std::function<void(IRLearnStatus)> m_onStatusChange;   // 状态变化回调
    std::function<void(const std::vector<uint8_t>&)> m_onExtLearnComplete;  // 外部学习完成回调（原始红外码）

    bool m_isExtLearning;  // 是否正在外部学习

    // 串口操作
    bool setupSerial(int baudRate = 115200);
    bool writeFrame(const std::vector<uint8_t>& data);
    bool readFrame(std::vector<uint8_t>& outData, int timeoutMs = 5000);

    // 协议处理
    std::vector<uint8_t> buildFrame(uint8_t cmd, const std::vector<uint8_t>& data);
    bool parseFrame(const std::vector<uint8_t>& frame, uint8_t& cmd, std::vector<uint8_t>& data);
    uint8_t calcChecksum(const std::vector<uint8_t>& data, size_t start, size_t len);
    bool tryParseFrameFromBuffer(std::vector<uint8_t>& outData); // 从缓冲区解析帧

    // 接收线程
    bool m_running;
    std::unique_ptr<std::thread> m_receiveThread;
    std::mutex m_readMutex;
    std::vector<uint8_t> m_rxPending;
    void receiveLoop();

  public:
    static IRDevice& getInstance();

    explicit IRDevice(const std::string& path = "/dev/ttyS1");
    ~IRDevice();

    // 设备操作
    bool open();
    void close();
    bool isOpen() const;

    // 学习功能
    bool startLearn(uint8_t index);
    bool stopLearn();
    IRLearnStatus getLearnStatus() const;
    IRCode getLastLearnedCode() const;

    // 发射功能
    bool emitCode(uint8_t index);
    bool emitRawCode(const std::vector<uint8_t>& code);

    // 存储管理
    bool clearCode(uint8_t index);
    bool clearAllCodes();

    // 外部学习模式（数据直接返回给主控，不存储在模块）
    bool startExtLearn();
    bool stopExtLearn();
    bool isExtLearning() const;

    // 回调设置
    void setOnLearnComplete(std::function<void(const IRCode&)> callback);
    void setOnStatusChange(std::function<void(IRLearnStatus)> callback);
    void setOnExtLearnComplete(std::function<void(const std::vector<uint8_t>&)> callback);
};

#endif // LVGL_APP_HAL_IRDEVICE_H
