#ifndef LVGL_APP_IR_COMMAND_MANAGER_H
#define LVGL_APP_IR_COMMAND_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct IRCommand
{
    std::string deviceName;
    std::string commandName;
    std::vector<uint8_t> data;
    uint32_t timestamp;
};

class IRCommandManager
{
public:
    static IRCommandManager &getInstance();

    bool init(const std::string &dataPath = "/data/ir_commands");
    void deinit();

    bool addCommand(const std::string &deviceName, const std::string &commandName, const std::vector<uint8_t> &data);
    bool removeCommand(const std::string &deviceName, const std::string &commandName);
    bool removeDevice(const std::string &deviceName);

    std::vector<uint8_t> getCommand(const std::string &deviceName, const std::string &commandName);
    bool emitCommand(const std::string &deviceName, const std::string &commandName);

    std::vector<std::string> getDeviceList();
    std::vector<std::string> getCommandList(const std::string &deviceName);

    bool saveToFile();
    bool loadFromFile();

    bool commandExists(const std::string &deviceName, const std::string &commandName);

private:
    IRCommandManager();
    ~IRCommandManager();
    IRCommandManager(const IRCommandManager &) = delete;
    IRCommandManager &operator=(const IRCommandManager &) = delete;

    std::string getKey(const std::string &deviceName, const std::string &commandName);

    std::string m_dataPath;
    std::unordered_map<std::string, IRCommand> m_commands;
    bool m_initialized;
};

#endif
