/**
 * @file IRDevice.cpp
 * @brief 红外学习模块驱动实现
 * @details 实现串口通信、协议解析和红外码学习/发射功能
 */

#include "IRDevice.h"
#include <chrono>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

/**
 * @brief 获取单例实例
 * @return IRDevice& 单例引用
 */
IRDevice& IRDevice::getInstance()
{
    static IRDevice instance;
    return instance;
}

/**
 * @brief 构造函数
 * @param path 串口设备路径
 */
IRDevice::IRDevice(const std::string& path)
    : m_devPath(path)
    , m_fd(-1)
    , m_isLearning(false)
    , m_learnStatus(IRLearnStatus::IDLE)
    , m_running(false)
    , m_isExtLearning(false)
{
}

/**
 * @brief 析构函数
 */
IRDevice::~IRDevice()
{
    close();
}

/**
 * @brief 打开设备
 * @return true 打开成功
 * @return false 打开失败
 */
bool IRDevice::open()
{
    if (m_fd >= 0)
        return true;

    m_fd = ::open(m_devPath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_fd < 0)
    {
        std::cerr << "Failed to open IR device: " << m_devPath << std::endl;
        return false;
    }

    if (!setupSerial(115200))
    {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    m_running = true;
    m_receiveThread = std::make_unique<std::thread>(&IRDevice::receiveLoop, this);
    std::cout << "[IRDevice] 接收线程已启动" << std::endl;
    return true;
}

/**
 * @brief 关闭设备
 */
void IRDevice::close()
{
    m_running = false;
    if (m_receiveThread && m_receiveThread->joinable())
    {
        m_receiveThread->join();
    }
    m_receiveThread.reset();
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
    m_isLearning = false;
    m_learnStatus = IRLearnStatus::IDLE;
}

/**
 * @brief 检查设备是否打开
 * @return true 已打开
 * @return false 未打开
 */
bool IRDevice::isOpen() const
{
    return m_fd >= 0;
}

/**
 * @brief 配置串口参数
 * @param baudRate 波特率
 * @return true 配置成功
 * @return false 配置失败
 */
bool IRDevice::setupSerial(int baudRate)
{
    struct termios options;
    if (tcgetattr(m_fd, &options) != 0)
    {
        std::cerr << "tcgetattr failed" << std::endl;
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

    tcflush(m_fd, TCIFLUSH);

    if (tcsetattr(m_fd, TCSANOW, &options) != 0)
    {
        std::cerr << "tcsetattr failed" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief 计算校验和
 * @param data 数据数组
 * @param start 起始位置
 * @param len 长度
 * @return uint8_t 校验和
 */
uint8_t IRDevice::calcChecksum(const std::vector<uint8_t>& data, size_t start, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = start; i < start + len && i < data.size(); i++)
    {
        sum += data[i];
    }
    return sum;
}

/**
 * @brief 构建协议帧
 * @param cmd 命令码
 * @param data 数据内容
 * @return std::vector<uint8_t> 完整帧数据
 * @details 格式: 68H + 长度(2字节小端) + 模块地址 + 功能码 + 数据 + 校验 + 16H
 *          长度 = 1(68) + 2(长度) + 1(模块地址) + 1(功能码) + N(数据) + 1(校验) + 1(16H)
 *          校验 = (模块地址 + 功能码 + 数据域) % 256
 */
std::vector<uint8_t> IRDevice::buildFrame(uint8_t cmd, const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> frame;
    frame.push_back(IR_FRAME_HEAD);

    uint16_t totalLen = 1 + 2 + 1 + 1 + data.size() + 1 + 1;
    frame.push_back(totalLen & 0xFF);  // 长度低字节
    frame.push_back((totalLen >> 8) & 0xFF);  // 长度高字节
    frame.push_back(IR_ADDR_BROADCAST);  // 模块地址（广播）
    frame.push_back(cmd);  // 功能码

    for (auto byte : data)
    {
        frame.push_back(byte);
    }

    // 校验 = 模块地址 + 功能码 + 数据域
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
 * @details 格式: 68H + 长度(2字节小端) + 模块地址 + 功能码 + 数据 + 校验 + 16H
 */
bool IRDevice::parseFrame(const std::vector<uint8_t>& frame, uint8_t& cmd, std::vector<uint8_t>& data)
{
    if (frame.size() < 7)  // 最小帧长度: 1+2+1+1+0+1+1=7
        return false;

    if (frame[0] != IR_FRAME_HEAD || frame[frame.size() - 1] != IR_FRAME_TAIL)
        return false;

    // 解析长度(小端)
    uint16_t expectedLen = frame[1] | (static_cast<uint16_t>(frame[2]) << 8);
    if (frame.size() != expectedLen)
        return false;

    // 校验 = 模块地址 + 功能码 + 数据域
    size_t dataLen = expectedLen - 7;  // 总长 - 固定7字节
    uint8_t checksum = calcChecksum(frame, 3, 1 + 1 + dataLen);  // 模块地址+功能码+数据
    if (checksum != frame[expectedLen - 2])
        return false;

    cmd = frame[4];  // 功能码位置
    data.clear();
    for (size_t i = 5; i < expectedLen - 2; i++)
    {
        data.push_back(frame[i]);
    }

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
    if (m_fd < 0)
        return false;

    std::cout << "[发送] 协议帧: ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (int)data[i] << " ";
    }
    std::cout << std::dec << std::endl;

    ssize_t written = write(m_fd, data.data(), data.size());
    if (written != static_cast<ssize_t>(data.size()))
    {
        std::cerr << "Write frame failed" << std::endl;
        return false;
    }

    tcdrain(m_fd);
    return true;
}

/**
 * @brief 从串口读取帧数据
 * @param outData 输出数据
 * @param timeoutMs 超时时间(毫秒)
 * @return true 读取成功
 * @return false 读取失败或超时
 */
bool IRDevice::readFrame(std::vector<uint8_t>& outData, int timeoutMs)
{
    std::lock_guard<std::mutex> lock(m_readMutex);

    if (m_fd < 0)
        return false;

    outData.clear();

    const auto start = std::chrono::steady_clock::now();

    while (true)
    {
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - start)
                             .count();
        int remainingMs = timeoutMs - static_cast<int>(elapsedMs);
        if (remainingMs <= 0)
        {
            if (m_isExtLearning && !m_rxPending.empty())
            {
                outData = m_rxPending;
                m_rxPending.clear();
                return true;
            }
            return false;
        }

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_fd, &readfds);

        struct timeval tv;
        tv.tv_sec = remainingMs / 1000;
        tv.tv_usec = (remainingMs % 1000) * 1000;

        int ret = select(m_fd + 1, &readfds, nullptr, nullptr, &tv);
        if (ret < 0)
        {
            if (errno == EINTR)
                continue;
            return false;
        }
        if (ret == 0)
            continue;

        uint8_t temp[256];
        ssize_t bytesRead = read(m_fd, temp, sizeof(temp));
        if (bytesRead < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                continue;
            return false;
        }
        if (bytesRead == 0)
            continue;

        m_rxPending.insert(m_rxPending.end(), temp, temp + bytesRead);

        if (!m_isExtLearning)
        {
            while (!m_rxPending.empty() && m_rxPending.front() != IR_FRAME_HEAD)
            {
                m_rxPending.erase(m_rxPending.begin());
            }
        }

        while (m_rxPending.size() >= 3)
        {
            if (m_isExtLearning && m_rxPending.front() != IR_FRAME_HEAD)
            {
                break;
            }

            if (m_rxPending[0] != IR_FRAME_HEAD)
            {
                m_rxPending.erase(m_rxPending.begin());
                continue;
            }

            uint16_t expectedLen = m_rxPending[1] | (static_cast<uint16_t>(m_rxPending[2]) << 8);
            if (expectedLen < 7 || expectedLen > IR_MAX_CODE_LENGTH)
            {
                m_rxPending.erase(m_rxPending.begin());
                continue;
            }

            if (m_rxPending.size() < expectedLen)
                break;

            if (m_rxPending[expectedLen - 1] == IR_FRAME_TAIL)
            {
                outData.assign(m_rxPending.begin(), m_rxPending.begin() + expectedLen);
                m_rxPending.erase(m_rxPending.begin(), m_rxPending.begin() + expectedLen);
                return true;
            }

            m_rxPending.erase(m_rxPending.begin());
        }

        if (m_rxPending.size() > IR_MAX_CODE_LENGTH)
        {
            m_rxPending.clear();
        }
    }
}

/**
 * @brief 开始学习
 * @param index 存储索引(0-255)
 * @return true 命令发送成功
 * @return false 命令发送失败
 */
bool IRDevice::startLearn(uint8_t index)
{
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_LEARN, data);

    if (!writeFrame(frame))
        return false;

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
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_STOP_LEARN, data);

    if (!writeFrame(frame))
        return false;

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
 * @return IRLearnStatus 当前状态
 */
IRLearnStatus IRDevice::getLearnStatus() const
{
    return m_learnStatus;
}

/**
 * @brief 获取最后学习的红外码
 * @return IRCode 红外码数据
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
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_EMIT, data);

    if (!writeFrame(frame))
        return false;

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            if (cmd == IR_CMD_EMIT && respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS)
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief 发射原始红外码
 * @param code 红外码数据（可以是完整帧或原始数据）
 * @return true 发射成功
 * @return false 发射失败
 */
bool IRDevice::emitRawCode(const std::vector<uint8_t>& code)
{
    if (m_fd < 0 || code.empty())
        return false;

    // 检查是否是完整的帧数据（以0x68开头，以0x16结尾）
    if (code.size() >= 4 && code[0] == IR_FRAME_HEAD && code.back() == IR_FRAME_TAIL)
    {
        // 直接发送完整帧
        std::cout << "[发射] 发送完整帧，长度: " << code.size() << std::endl;
        return writeFrame(code);
    }
    else
    {
        // 构建帧并发送
        if (code.size() > IR_MAX_CODE_LENGTH)
            return false;

        auto frame = buildFrame(IR_CMD_EXT_EMIT, code);
        std::cout << "[发射] 构建帧发送，数据长度: " << code.size() << std::endl;
        return writeFrame(frame);
    }
}

/**
 * @brief 清除指定索引的红外码
 * @param index 存储索引
 * @return true 清除成功
 * @return false 清除失败
 */
bool IRDevice::clearCode(uint8_t index)
{
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data = {index};
    auto frame = buildFrame(IR_CMD_CLEAR, data);

    if (!writeFrame(frame))
        return false;

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            if (cmd == IR_CMD_CLEAR && respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS)
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
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_CLEAR_ALL, data);

    if (!writeFrame(frame))
        return false;

    std::vector<uint8_t> response;
    if (readFrame(response, 1000))
    {
        uint8_t cmd;
        std::vector<uint8_t> respData;
        if (parseFrame(response, cmd, respData))
        {
            if (cmd == IR_CMD_CLEAR_ALL && respData.size() >= 2 && respData[1] == IR_ACK_SUCCESS)
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief 设置学习完成回调
 * @param callback 回调函数
 */
void IRDevice::setOnLearnComplete(std::function<void(const IRCode&)> callback)
{
    m_onLearnComplete = callback;
}

/**
 * @brief 设置状态变化回调
 * @param callback 回调函数
 */
void IRDevice::setOnStatusChange(std::function<void(IRLearnStatus)> callback)
{
    m_onStatusChange = callback;
}

/**
 * @brief 接收线程循环
 * @details 持续监听串口，处理模块主动上报的帧
 */
void IRDevice::receiveLoop()
{
    std::vector<uint8_t> frame;

    std::cout << "[IRDevice] 接收线程开始运行" << std::endl;

    while (m_running)
    {
        if (readFrame(frame, m_isExtLearning ? 300 : 100))
        {
            std::cout << "[接收线程] 协议帧: ";
            for (auto b : frame)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
            }
            std::cout << std::dec << std::endl;

            uint8_t cmd;
            std::vector<uint8_t> data;
            if (parseFrame(frame, cmd, data))
            {
                std::cout << "[解析] 命令码: 0x" << std::hex << (int)cmd 
                          << ", 数据长度: " << std::dec << data.size() << std::endl;

                // 处理学习完成上报 (命令码 0x02)
                if (cmd == IR_ACK_LEARN_COMPLETE && m_isLearning)
                {
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
                // 外部学习流程：
                // 1. 发送 AFN=20H 进入外部学习模式
                // 2. 模块回复应答帧 AFN=01H（确认进入学习模式，绿灯亮）
                // 3. 用户按下遥控器后，模块主动发送 AFN=22H 的数据帧
                else if (m_isExtLearning)
                {
                    std::cout << "[外部学习] 收到帧，cmd=0x" << std::hex << (int)cmd << std::dec << std::endl;
                    
                    // AFN=22H 是外部学习的数据帧，包含红外码数据
                    if (cmd == IR_CMD_EXT_EMIT)
                    {
                        std::cout << "[外部学习] 收到红外码数据，长度: " << data.size() << std::endl;
                        
                        m_isExtLearning = false;
                        m_learnStatus = IRLearnStatus::SUCCESS;

                        if (m_onStatusChange)
                        {
                            m_onStatusChange(m_learnStatus);
                        }

                        if (m_onExtLearnComplete)
                        {
                            // 保存整个帧数据（用于后续发射）
                            m_onExtLearnComplete(frame);
                        }
                    }
                    // AFN=01H 是成功应答，表示模块已进入外部学习模式
                    else if (cmd == IR_ACK_SUCCESS)
                    {
                        std::cout << "[外部学习] 模块已进入学习模式（绿灯亮），请按下遥控器..." << std::endl;
                        // 继续等待遥控器信号
                    }
                    // AFN=20H 是进入外部学习模式的应答（备用）
                    else if (cmd == IR_CMD_EXT_LEARN)
                    {
                        std::cout << "[外部学习] 模块已进入学习模式，等待遥控器信号..." << std::endl;
                        // 继续等待，不改变状态
                    }
                    else
                    {
                        std::cout << "[外部学习] 收到其他命令码: 0x" << std::hex << (int)cmd << std::dec << std::endl;
                    }
                }
            }
            else if (m_isExtLearning && !frame.empty())
            {
                std::cout << "[外部学习] 收到原始波形数据，长度: " << frame.size() << std::endl;

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
            frame.clear();
        }
    }
}

/**
 * @brief 开始外部学习模式
 * @return true 命令发送成功
 * @return false 命令发送失败
 */
bool IRDevice::startExtLearn()
{
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_EXT_LEARN, data);

    if (!writeFrame(frame))
        return false;

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
    if (m_fd < 0)
        return false;

    std::vector<uint8_t> data;
    auto frame = buildFrame(IR_CMD_EXT_STOP_LEARN, data);

    if (!writeFrame(frame))
        return false;

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
bool IRDevice::isExtLearning() const
{
    return m_isExtLearning;
}

/**
 * @brief 设置外部学习完成回调
 * @param callback 回调函数，参数为原始红外码数据
 */
void IRDevice::setOnExtLearnComplete(std::function<void(const std::vector<uint8_t>&)> callback)
{
    m_onExtLearnComplete = callback;
}
