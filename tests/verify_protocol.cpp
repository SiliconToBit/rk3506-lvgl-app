
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

constexpr uint8_t IR_FRAME_HEAD = 0x68;
constexpr uint8_t IR_FRAME_TAIL = 0x16;
constexpr uint8_t IR_ADDR_BROADCAST = 0xFF;
constexpr uint8_t IR_CMD_LEARN = 0x10;
constexpr uint8_t IR_CMD_EMIT = 0x12;

void printFrame(const std::vector<uint8_t>& frame) {
    std::cout << "协议帧: ";
    for (size_t i = 0; i < frame.size(); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (int)frame[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

std::vector<uint8_t> buildTestFrame(uint8_t cmd, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> frame;
    frame.push_back(IR_FRAME_HEAD);

    uint16_t totalLen = 1 + 2 + 1 + 1 + data.size() + 1 + 1;
    frame.push_back(totalLen & 0xFF);
    frame.push_back((totalLen >> 8) & 0xFF);
    frame.push_back(IR_ADDR_BROADCAST);
    frame.push_back(cmd);

    for (auto byte : data) {
        frame.push_back(byte);
    }

    uint8_t checksum = 0;
    for (size_t i = 3; i < 3 + 1 + 1 + data.size(); i++) {
        checksum += frame[i];
    }
    frame.push_back(checksum);
    frame.push_back(IR_FRAME_TAIL);

    return frame;
}

int main() {
    std::cout << "--- 学习命令 (索引 0) ---" << std::endl;
    std::vector<uint8_t> data1 = {0x00};
    auto frame1 = buildTestFrame(IR_CMD_LEARN, data1);
    printFrame(frame1);
    std::cout << "说明书示例: 68 08 00 FF 10 00 0F 16" << std::endl << std::endl;

    std::cout << "--- 发射命令 (索引 0) ---" << std::endl;
    std::vector<uint8_t> data2 = {0x00};
    auto frame2 = buildTestFrame(IR_CMD_EMIT, data2);
    printFrame(frame2);
    std::cout << "说明书示例: 68 08 00 FF 12 00 11 16" << std::endl;

    return 0;
}
