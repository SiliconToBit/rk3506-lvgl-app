/**
 * @file IRDevice.cpp
 * @brief 红外学习模块驱动实现
 *
 * 实现串口通信、协议解析和红外码学习/发射功能
 */

#include "IRDevice.h"

#include <chrono>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

namespace chrono = std::chrono;

/**
 * @brief 获取单例实例
 * @return IRDevice& 单例引用
 *
 * 使用静态局部变量实现线程安全的单例模式（Meyer's Singleton）
 */
IRDevice& IRDevice::getInstance()
{
    static IRDevice instance;
    return instance;
}

/**
 * @brief 构造函数
 * @param path 串口设备路径
 *
 * 仅保存设备路径，不打开设备（延迟初始化）
 */
IRDevice::IRDevice(std::string_view path)
    : m_devPath{path}
{
}

/**
 * @brief 析构函数
 *
 * 自动关闭设备
 */
IRDevice::~IRDevice()
{
    closeDevice();
}

/**
 * @brief 打开设备
 * @return true 打开成功
 * @return false 打开失败
 *
 * 打开串口设备并配置串口参数
 */
bool IRDevice::openDevice()
{
    if (m_fd.isValid())
    {
        return true;
    }

    m_fd = FileDescriptor{m_devPath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY};
    if (!m_fd.isValid())
    {
        std::cerr << "[IRDevice] Failed to open device: " << m_devPath << '\n';
        return false;
    }

    if (!setupSerial(115200))
    {
        m_fd = FileDescriptor{};
        return false;
    }

    // 清空串口缓冲区
    tcflush(m_fd.get(), TCIOFLUSH);

    std::cout << "[IRDevice] Device opened\n";
    return true;
}

/**
 * @brief 关闭设备
 *
 * 停止接收线程并关闭串口设备
 */
void IRDevice::closeDevice() noexcept
{
    std::cout << "[IRDevice] close() called\n";
    m_running = false;
    std::cout << "[IRDevice] m_running set to false\n";

    if (m_receiveThread && m_receiveThread->joinable())
    {
        std::cout << "[IRDevice] Joining receive thread...\n";
        m_receiveThread->join();
        std::cout << "[IRDevice] Receive thread joined\n";
    }
    m_receiveThread.reset();

    m_fd = FileDescriptor{};
    m_isLearning = false;
    m_learnStatus = IRLearnStatus::IDLE;
    std::cout << "[IRDevice] close() done\n";
}

/**
 * @brief 检查设备是否打开
 * @return true 已打开
 * @return false 未打开
 */
bool IRDevice::isOpen() const noexcept
{
    return m_fd.isValid();
}

/**
 * @brief 配置串口参数
 * @param baudRate 波特率
 * @return true 配置成功
 * @return false 配置失败
 */
bool IRDevice::setupSerial(int baudRate)
{
    struct termios options{};
    if (tcgetattr(m_fd.get(), &options) != 0)
    {
        std::cerr << "[IRDevice] tcgetattr failed\n";
        return false;
    }

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    tcflush(m_fd.get(), TCIFLUSH);

    if (tcsetattr(m_fd.get(), TCSANOW, &options) != 0)
    {
        std::cerr << "[IRDevice] tcsetattr failed\n";
        return false;
    }

    return true;
}

/**
 * @brief 计算校验和
 * @param data 数据数组
 * @param start 起始位置
 * @param len 长度
 * @return 校验和
 */
uint8_t IRDevice::calcChecksum(const std::vector<uint8_t>& data, size_t start, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = start; i < start + len && i < data.size(); ++i)
    {
        sum += data[i];
    }
    return sum;
}

/**
 * @brief 构建协议帧
 * @param cmd 命令码
 * @param data 数据内容
 * @return 完整帧数据
 *
 * 格式: 68H + 长度(2字节小端) + 模块地址 + 功能码 + 数据 + 校验 + 16H
 * 长度 = 1(68) + 2(长度) + 1(模块地址) + 1(功能码) + N(数据) + 1(校验) + 1(16H)
 * 校验 = (模块地址 + 功能码 + 数据域) % 256
 */
std::vector<uint8_t> IRDevice::buildFrame(uint8_t cmd, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> frame;
    frame.reserve(7 + data.size());

    frame.push_back(IR_FRAME_HEAD);

    uint16_t totalLen = static_cast<uint16_t>(1 + 2 + 1 + 1 + data.size() + 1 + 1);
    frame.push_back(static_cast<uint8_t>(totalLen & 0xFF));
    frame.push_back(static_cast<uint8_t>((totalLen >> 8) & 0xFF));
    frame.push_back(IR_ADDR_BROADCAST);
    frame.push_back(cmd);

    frame.insert(frame.end(), data.begin(), data.end());

    uint8_t checksum = calcChecksum(frame, 3, 1 + 1 + data.size());
    frame.push_back(checksum);
    frame.push_back(IR_FRAME_TAIL);

    return frame;
}

/**
 * @brief 解析协议帧
 * @param frame 接收到的帧数据
 * @param cmd 输出命令码
 * @param data 输出数据内容
 * @return true 解析成功
 * @return false 解析失败
 */
bool IRDevice::parseFrame(const std::vector<uint8_t>& frame, uint8_t& cmd, std::vector<uint8_t>& data)
{
    if (frame.size() < 7)
    {
        return false;
    }

    if (frame[0] != IR_FRAME_HEAD || frame[frame.size() - 1] != IR_FRAME_TAIL)
    {
        return false;
    }

    uint16_t expectedLen = static_cast<uint16_t>(frame[1] | (static_cast<uint16_t>(frame[2]) << 8));
    if (frame.size() != expectedLen)
    {
        return false;
    }

    size_t dataLen = expectedLen - 7;
    uint8_t checksum = calcChecksum(frame, 3, 1 + 1 + dataLen);
    if (checksum != frame[expectedLen - 2])
    {
        return false;
    }

    cmd = frame[4];
    data.clear();
    data.insert(data.end(), frame.begin() + 5, frame.begin() + expectedLen - 2);

    return true;
}

/**
 * @brief 写入帧数据到串口
 * @param data 帧数据
 * @return true 写入成功
 * @return false 写入失败
 */
bool IRDevice::writeFrame(const std::vector<uint8_t>& data)
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::cout << "[发送] 协议帧: ";
    for (const auto byte : data)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << '\n';

    ssize_t written = m_fd.write(data.data(), data.size());
    if (written != static_cast<ssize_t>(data.size()))
    {
        std::cerr << "[IRDevice] Write frame failed\n";
        return false;
    }

    tcdrain(m_fd.get());
    return true;
}

/**
 * @brief 从串口读取帧数据
 * @param outData 输出数据
 * @param timeoutMs 超时时间（毫秒）
 * @return true 读取成功
 * @return false 读取失败或超时
 */
bool IRDevice::readFrame(std::vector<uint8_t>& outData, int timeoutMs)
{
    if (!m_fd.isValid())
    {
        return false;
    }

    outData.clear();

    const auto start = chrono::steady_clock::now();

    while (true)
    {
        auto elapsedMs = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
        int remainingMs = timeoutMs - static_cast<int>(elapsedMs);
        if (remainingMs <= 0)
        {
            return false;
        }

        if (tryParseFrameFromBuffer(outData))
        {
            return true;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_fd.get(), &readfds);

        struct timeval tv{};
        tv.tv_sec = remainingMs / 1000;
        tv.tv_usec = (remainingMs % 1000) * 1000;

        int ret = select(m_fd.get() + 1, &readfds, nullptr, nullptr, &tv);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return false;
        }
        if (ret == 0)
        {
            continue;
        }

        uint8_t temp[256];
        ssize_t bytesRead = m_fd.read(temp, sizeof(temp));
        if (bytesRead < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            {
                continue;
            }
            return false;
        }
        if (bytesRead == 0)
        {
            continue;
        }

        m_rxPending.insert(m_rxPending.end(), temp, temp + bytesRead);

        // 清理无效数据
        while (!m_rxPending.empty() && m_rxPending.front() != IR_FRAME_HEAD)
        {
            m_rxPending.erase(m_rxPending.begin());
        }

        if (tryParseFrameFromBuffer(outData))
        {
            return true;
        }

        if (m_rxPending.size() > IR_MAX_CODE_LENGTH * 2)
        {
            m_rxPending.clear();
        }
    }
}

/**
 * @brief 从缓冲区解析帧
 * @param outData 输出数据
 * @return true 解析成功
 * @return false 解析失败
 */
bool IRDevice::tryParseFrameFromBuffer(std::vector<uint8_t>& outData)
{
    while (m_rxPending.size() >= 3)
    {
        // 外部学习模式下，允许非标准帧头
        if (m_isExtLearning && m_rxPending.front() != IR_FRAME_HEAD)
        {
            if (m_rxPending.size() > 100)
            {
                outData = m_rxPending;
                m_rxPending.clear();
                return true;
            }
            break;
        }

        if (m_rxPending[0] != IR_FRAME_HEAD)
        {
            m_rxPending.erase(m_rxPending.begin());
            continue;
        }

        uint16_t expectedLen = static_cast<uint16_t>(m_rxPending[1] | (static_cast<uint16_t>(m_rxPending[2]) << 8));
        if (expectedLen < 7 || expectedLen > IR_MAX_CODE_LENGTH)
        {
            m_rxPending.erase(m_rxPending.begin());
            continue;
        }

        if (m_rxPending.size() < expectedLen)
        {
            break;
        }

        if (m_rxPending[expectedLen - 1] == IR_FRAME_TAIL)
        {
            outData.assign(m_rxPending.begin(), m_rxPending.begin() + expectedLen);
            m_rxPending.erase(m_rxPending.begin(), m_rxPending.begin() + expectedLen);
            return true;
        }

        m_rxPending.erase(m_rxPending.begin());
    }

    return false;
}

/**
 * @brief 开始学习
 * @param index 存储索引（0-255）
 * @return true 命令发送成功
 * @return false 命令发送失败
 */
bool IRDevice::startLearn(uint8_t index)
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_LEARN, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    m_isLearning = true;
    m_learnStatus = IRLearnStatus::LEARNING;

    if (m_onStatusChange)
    {
        m_onStatusChange(m_learnStatus);
    }

    return true;
}

/**
 * @brief 停止学习
 * @return true 停止成功
 * @return false 停止失败
 */
bool IRDevice::stopLearn()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_STOP_LEARN, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    m_isLearning = false;
    m_learnStatus = IRLearnStatus::IDLE;

    if (m_onStatusChange)
    {
        m_onStatusChange(m_learnStatus);
    }

    return true;
}

/**
 * @brief 获取学习状态
 * @return 当前状态
 */
IRLearnStatus IRDevice::getLearnStatus() const noexcept
{
    return m_learnStatus;
}

/**
 * @brief 获取最后学习的红外码
 * @return 红外码数据
 */
IRCode IRDevice::getLastLearnedCode() const
{
    return m_lastLearnedCode;
}

/**
 * @brief 发射指定索引的红外码
 * @param index 存储索引
 * @return true 发射成功
 * @return false 发射失败
 */
bool IRDevice::emitCode(uint8_t index)
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_EMIT, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd = 0;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            const bool ackSuccess = (!respData.empty() && respData[0] == IR_ACK_SUCCESS) ||
                                    (respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS);
            if (cmd == IR_CMD_EMIT && ackSuccess)
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief 发射原始红外码
 * @param code 红外码数据
 * @return true 发射成功
 * @return false 发射失败
 */
bool IRDevice::emitRawCode(const std::vector<uint8_t>& code)
{
    if (!m_fd.isValid() || code.empty())
    {
        return false;
    }

    // 清空接收缓冲区
    m_rxPending.clear();
    tcflush(m_fd.get(), TCIFLUSH);

    // 检查是否是完整的帧数据
    if (code.size() >= 4 && code[0] == IR_FRAME_HEAD && code.back() == IR_FRAME_TAIL)
    {
        std::cout << "[发射] 发送完整帧，长度: " << code.size() << '\n';
        if (!writeFrame(code))
        {
            return false;
        }
    }
    else
    {
        if (code.size() > IR_MAX_CODE_LENGTH)
        {
            return false;
        }

        auto frame = buildFrame(IR_CMD_EXT_EMIT, code);
        std::cout << "[发射] 构建帧发送，数据长度: " << code.size() << '\n';
        if (!writeFrame(frame))
        {
            return false;
        }
    }

    // 等待并读取模块应答
    const auto deadline = chrono::steady_clock::now() + chrono::milliseconds(500);
    while (chrono::steady_clock::now() < deadline)
    {
        int remainMs = static_cast<int>(
            chrono::duration_cast<chrono::milliseconds>(deadline - chrono::steady_clock::now()).count());
        if (remainMs <= 0)
        {
            break;
        }

        std::vector<uint8_t> response;
        if (!readFrame(response, remainMs))
        {
            break;
        }

        uint8_t cmd = 0;
        std::vector<uint8_t> respData;
        if (!parseFrame(response, cmd, respData))
        {
            continue;
        }

        if (respData.size() > 2)
        {
            continue;
        }

        const bool ackSuccess = (!respData.empty() && respData[0] == IR_ACK_SUCCESS) ||
                                (respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS);
        if (ackSuccess)
        {
            std::cout << "[发射] 模块应答成功\n";
            return true;
        }

        const bool ackFail =
            (!respData.empty() && respData[0] == IR_ACK_FAIL) || (respData.size() >= 2 && respData[1] == IR_ACK_FAIL);
        if (ackFail)
        {
            std::cerr << "[发射] 模块应答失败\n";
            return false;
        }
    }

    std::cout << "[发射] 等待应答超时，假设发送成功\n";
    return true;
}

/**
 * @brief 清除指定索引的红外码
 * @param index 存储索引
 * @return true 清除成功
 * @return false 清除失败
 */
bool IRDevice::clearCode(uint8_t index)
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_CLEAR, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd = 0;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            const bool ackSuccess = (!respData.empty() && respData[0] == IR_ACK_SUCCESS) ||
                                    (respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS);
            if (cmd == IR_CMD_CLEAR && ackSuccess)
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief 清除所有红外码
 * @return true 清除成功
 * @return false 清除失败
 */
bool IRDevice::clearAllCodes()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_CLEAR_ALL, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd = 0;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            const bool ackSuccess = (!respData.empty() && respData[0] == IR_ACK_SUCCESS) ||
                                    (respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS);
            if (cmd == IR_CMD_CLEAR_ALL && ackSuccess)
            {
                return true;
            }
        }
    }

    return false;
}

/// 设置学习完成回调
void IRDevice::setOnLearnComplete(std::function<void(const IRCode&)> callback)
{
    m_onLearnComplete = std::move(callback);
}

/// 设置状态变化回调
void IRDevice::setOnStatusChange(std::function<void(IRLearnStatus)> callback)
{
    m_onStatusChange = std::move(callback);
}

/// 设置外部学习完成回调
void IRDevice::setOnExtLearnComplete(std::function<void(const std::vector<uint8_t>&)> callback)
{
    m_onExtLearnComplete = std::move(callback);
}

/**
 * @brief 接收线程循环
 *
 * 持续监听串口，处理模块主动上报的帧
 */
void IRDevice::receiveLoop()
{
    std::vector<uint8_t> frame;

    std::cout << "[IRDevice] 接收线程开始运行\n";

    while (m_running)
    {
        if (readFrame(frame, 50))
        {
            uint8_t cmd = 0;
            std::vector<uint8_t> data;
            bool parsed = parseFrame(frame, cmd, data);

            // 处理学习完成上报
            if (parsed && cmd == IR_ACK_LEARN_COMPLETE && m_isLearning)
            {
                std::cout << "[IR] 学习完成\n";
                m_isLearning = false;
                m_learnStatus = IRLearnStatus::SUCCESS;

                if (data.size() >= 1)
                {
                    m_lastLearnedCode.index = data[0];
                    if (data.size() > 1)
                    {
                        m_lastLearnedCode.data.assign(data.begin() + 1, data.end());
                    }
                }

                if (m_onStatusChange)
                {
                    m_onStatusChange(m_learnStatus);
                }
                if (m_onLearnComplete)
                {
                    m_onLearnComplete(m_lastLearnedCode);
                }
            }
            // 处理外部学习上报
            else if (m_isExtLearning)
            {
                if (parsed && cmd == IR_CMD_EXT_EMIT)
                {
                    std::cout << "[IR] 外部学习完成，数据长度: " << data.size() << '\n';
                    m_isExtLearning = false;
                    m_learnStatus = IRLearnStatus::SUCCESS;

                    if (m_onStatusChange)
                    {
                        m_onStatusChange(m_learnStatus);
                    }
                    if (m_onExtLearnComplete)
                    {
                        m_onExtLearnComplete(frame);
                    }
                }
            }

            frame.clear();
        }
    }

    std::cout << "[IRDevice] 接收线程已停止\n";
}

/**
 * @brief 开始外部学习模式
 * @return true 命令发送成功
 * @return false 命令发送失败
 */
bool IRDevice::startExtLearn()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_EXT_LEARN, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    m_isExtLearning = true;
    m_learnStatus = IRLearnStatus::LEARNING;

    if (m_onStatusChange)
    {
        m_onStatusChange(m_learnStatus);
    }

    return true;
}

/**
 * @brief 停止外部学习模式
 * @return true 命令发送成功
 * @return false 命令发送失败
 */
bool IRDevice::stopExtLearn()
{
    if (!m_fd.isValid())
    {
        return false;
    }

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_EXT_STOP_LEARN, data);

    if (!writeFrame(frame))
    {
        return false;
    }

    m_isExtLearning = false;
    m_learnStatus = IRLearnStatus::IDLE;

    if (m_onStatusChange)
    {
        m_onStatusChange(m_learnStatus);
    }

    return true;
}

/**
 * @brief 检查是否正在外部学习
 * @return true 正在外部学习
 * @return false 未在外部学习
 */
bool IRDevice::isExtLearning() const noexcept
{
    return m_isExtLearning;
}
