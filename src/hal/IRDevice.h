/**
 * @file IRDevice.h
 * @brief 红外学习模块驱动类
 *
 * 通过串口与红外学习模块通信，支持红外码学习和发射功能
 * 协议格式：帧头(68H) + 长度 + 地址 + 命令 + 数据 + 校验 + 帧尾(16H)
 */

#pragma once

#include "FileDescriptor.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

/// 帧头
constexpr uint8_t IR_FRAME_HEAD = 0x68;

/// 帧尾
constexpr uint8_t IR_FRAME_TAIL = 0x16;

/// 本机地址
constexpr uint8_t IR_ADDR_LOCAL = 0x00;

/// 广播地址
constexpr uint8_t IR_ADDR_BROADCAST = 0xFF;

/// 进入内部编码学习模式
constexpr uint8_t IR_CMD_LEARN = 0x10;

/// 退出内部编码学习模式
constexpr uint8_t IR_CMD_STOP_LEARN = 0x11;

/// 发送内部存储编码
constexpr uint8_t IR_CMD_EMIT = 0x12;

/// 读取内部存储编码
constexpr uint8_t IR_CMD_READ = 0x18;

/// 进入外部编码学习模式
constexpr uint8_t IR_CMD_EXT_LEARN = 0x20;

/// 退出外部编码学习模式
constexpr uint8_t IR_CMD_EXT_STOP_LEARN = 0x21;

/// 发送外部存储编码
constexpr uint8_t IR_CMD_EXT_EMIT = 0x22;

/// 格式化
constexpr uint8_t IR_CMD_FORMAT = 0x08;

/// 清除单个（备用）
constexpr uint8_t IR_CMD_CLEAR = 0x13;

/// 清除所有（备用）
constexpr uint8_t IR_CMD_CLEAR_ALL = 0x14;

/// 成功应答
constexpr uint8_t IR_ACK_SUCCESS = 0x01;

/// 失败应答
constexpr uint8_t IR_ACK_FAIL = 0x02;

/// 学习完成（上传）
constexpr uint8_t IR_ACK_LEARN_COMPLETE = 0x02;

/// 最大红外码长度
constexpr size_t IR_MAX_CODE_LENGTH = 1024;

/**
 * @brief 红外学习模块状态
 */
enum class IRLearnStatus
{
    IDLE,     ///< 空闲
    LEARNING, ///< 学习中
    SUCCESS,  ///< 学习成功
    TIMEOUT,  ///< 超时
    ERROR     ///< 错误
};

/**
 * @brief 红外码数据结构
 */
struct IRCode
{
    uint8_t index{0};          ///< 存储索引
    std::vector<uint8_t> data; ///< 红外码数据
    uint32_t timestamp{0};     ///< 学习时间戳
};

/**
 * @class IRDevice
 * @brief 红外学习模块驱动类
 *
 * 使用延迟初始化模式，需要调用 open() 打开设备后才能使用
 */
class IRDevice
{
public:
    /**
     * @brief 获取单例实例
     * @return IRDevice& 单例引用
     */
    [[nodiscard]] static IRDevice& getInstance();

    /**
     * @brief 构造函数
     * @param path 串口设备路径
     */
    explicit IRDevice(std::string_view path = "/dev/ttyS1");

    /// 析构函数
    ~IRDevice();

    // 禁用复制操作
    IRDevice(const IRDevice&) = delete;
    IRDevice& operator=(const IRDevice&) = delete;

    // 禁用移动操作（单例模式）
    IRDevice(IRDevice&&) = delete;
    IRDevice& operator=(IRDevice&&) = delete;

    /**
     * @brief 打开设备
     * @return true 打开成功
     * @return false 打开失败
     */
    [[nodiscard]] bool openDevice();

    /// 关闭设备
    void closeDevice() noexcept;

    /**
     * @brief 检查设备是否打开
     * @return true 已打开
     * @return false 未打开
     */
    [[nodiscard]] bool isOpen() const noexcept;

    /**
     * @brief 开始学习
     * @param index 存储索引（0-255）
     * @return true 命令发送成功
     * @return false 命令发送失败
     */
    [[nodiscard]] bool startLearn(uint8_t index);

    /**
     * @brief 停止学习
     * @return true 停止成功
     * @return false 停止失败
     */
    [[nodiscard]] bool stopLearn();

    /**
     * @brief 获取学习状态
     * @return 当前状态
     */
    [[nodiscard]] IRLearnStatus getLearnStatus() const noexcept;

    /**
     * @brief 获取最后学习的红外码
     * @return 红外码数据
     */
    [[nodiscard]] IRCode getLastLearnedCode() const;

    /**
     * @brief 发射指定索引的红外码
     * @param index 存储索引
     * @return true 发射成功
     * @return false 发射失败
     */
    [[nodiscard]] bool emitCode(uint8_t index);

    /**
     * @brief 发射原始红外码
     * @param code 红外码数据
     * @return true 发射成功
     * @return false 发射失败
     */
    [[nodiscard]] bool emitRawCode(const std::vector<uint8_t>& code);

    /**
     * @brief 清除指定索引的红外码
     * @param index 存储索引
     * @return true 清除成功
     * @return false 清除失败
     */
    [[nodiscard]] bool clearCode(uint8_t index);

    /**
     * @brief 清除所有红外码
     * @return true 清除成功
     * @return false 清除失败
     */
    [[nodiscard]] bool clearAllCodes();

    /**
     * @brief 开始外部学习模式
     * @return true 命令发送成功
     * @return false 命令发送失败
     */
    [[nodiscard]] bool startExtLearn();

    /**
     * @brief 停止外部学习模式
     * @return true 命令发送成功
     * @return false 命令发送失败
     */
    [[nodiscard]] bool stopExtLearn();

    /**
     * @brief 检查是否正在外部学习
     * @return true 正在外部学习
     * @return false 未在外部学习
     */
    [[nodiscard]] bool isExtLearning() const noexcept;

    /// 设置学习完成回调
    void setOnLearnComplete(std::function<void(const IRCode&)> callback);

    /// 设置状态变化回调
    void setOnStatusChange(std::function<void(IRLearnStatus)> callback);

    /// 设置外部学习完成回调
    void setOnExtLearnComplete(std::function<void(const std::vector<uint8_t>&)> callback);

private:
    std::string m_devPath;                            ///< 设备路径
    FileDescriptor m_fd;                              ///< 文件描述符
    bool m_isLearning{false};                         ///< 是否正在学习
    IRLearnStatus m_learnStatus{IRLearnStatus::IDLE}; ///< 学习状态
    IRCode m_lastLearnedCode;                         ///< 最后学习的红外码

    std::function<void(const IRCode&)> m_onLearnComplete;                  ///< 学习完成回调
    std::function<void(IRLearnStatus)> m_onStatusChange;                   ///< 状态变化回调
    std::function<void(const std::vector<uint8_t>&)> m_onExtLearnComplete; ///< 外部学习完成回调

    bool m_isExtLearning{false}; ///< 是否正在外部学习

    /// 配置串口参数
    [[nodiscard]] bool setupSerial(int baudRate = 115200);

    /// 写入帧数据
    [[nodiscard]] bool writeFrame(const std::vector<uint8_t>& data);

    /// 读取帧数据
    [[nodiscard]] bool readFrame(std::vector<uint8_t>& outData, int timeoutMs = 5000);

    /// 构建协议帧
    [[nodiscard]] std::vector<uint8_t> buildFrame(uint8_t cmd, const std::vector<uint8_t>& data);

    /// 解析协议帧
    [[nodiscard]] bool parseFrame(const std::vector<uint8_t>& frame, uint8_t& cmd, std::vector<uint8_t>& data);

    /// 计算校验和
    [[nodiscard]] static uint8_t calcChecksum(const std::vector<uint8_t>& data, size_t start, size_t len);

    /// 从缓冲区解析帧
    [[nodiscard]] bool tryParseFrameFromBuffer(std::vector<uint8_t>& outData);

    bool m_running{false};                        ///< 接收线程运行标志
    std::unique_ptr<std::thread> m_receiveThread; ///< 接收线程
    std::mutex m_readMutex;                       ///< 读取互斥锁
    std::vector<uint8_t> m_rxPending;             ///< 接收缓冲区

    /// 接收线程循环
    void receiveLoop();
};
