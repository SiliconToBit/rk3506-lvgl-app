#include "IRCommandManager.h"
#include "IRDevice.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <cerrno>
#include <iostream>

namespace
{
    bool ensureDirRecursive(const std::string& path)
    {
        if (path.empty())
            return false;

        if (path == "/")
            return true;

        struct stat st;
        if (stat(path.c_str(), &st) == 0)
        {
            return S_ISDIR(st.st_mode);
        }

        const size_t pos = path.find_last_of('/');
        if (pos != std::string::npos && pos > 0)
        {
            std::string parent = path.substr(0, pos);
            if (!parent.empty() && !ensureDirRecursive(parent))
            {
                return false;
            }
        }

        if (mkdir(path.c_str(), 0755) == 0)
        {
            return true;
        }

        if (errno == EEXIST)
        {
            return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
        }

        return false;
    }
} // namespace

IRCommandManager::IRCommandManager() : m_initialized(false) {}

IRCommandManager::~IRCommandManager()
{
    deinit();
}

IRCommandManager& IRCommandManager::getInstance()
{
    static IRCommandManager instance;
    return instance;
}

bool IRCommandManager::init(const std::string& dataPath)
{
    if (m_initialized)
        return true;

    m_dataPath = dataPath;

    if (!ensureDirRecursive(m_dataPath))
    {
        return false;
    }

    m_initialized = true;
    loadFromFile();

    return true;
}

void IRCommandManager::deinit()
{
    if (!m_initialized)
        return;

    saveToFile();
    m_commands.clear();
    m_initialized = false;
}

std::string IRCommandManager::getKey(const std::string& deviceName, const std::string& commandName)
{
    return deviceName + "::" + commandName;
}

bool IRCommandManager::addCommand(const std::string& deviceName, const std::string& commandName,
                                  const std::vector<uint8_t>& data)
{
    if (!m_initialized)
        return false;

    std::string key = getKey(deviceName, commandName);

    IRCommand cmd;
    cmd.deviceName = deviceName;
    cmd.commandName = commandName;
    cmd.data = data;
    cmd.timestamp = static_cast<uint32_t>(time(nullptr));

    m_commands[key] = cmd;
    return saveToFile();
}

bool IRCommandManager::removeCommand(const std::string& deviceName, const std::string& commandName)
{
    if (!m_initialized)
        return false;

    std::string key = getKey(deviceName, commandName);
    auto it = m_commands.find(key);
    if (it == m_commands.end())
        return false;

    IRCommand backup = it->second;
    m_commands.erase(it);
    if (saveToFile())
        return true;

    m_commands[key] = backup;
    return false;
}

bool IRCommandManager::removeDevice(const std::string& deviceName)
{
    if (!m_initialized)
        return false;

    std::unordered_map<std::string, IRCommand> backup = m_commands;
    std::vector<std::string> keysToRemove;
    for (auto& pair : m_commands)
    {
        if (pair.second.deviceName == deviceName)
        {
            keysToRemove.push_back(pair.first);
        }
    }

    for (auto& key : keysToRemove)
    {
        m_commands.erase(key);
    }

    if (keysToRemove.empty())
        return false;

    if (saveToFile())
        return true;

    m_commands = std::move(backup);
    return false;
}

std::vector<uint8_t> IRCommandManager::getCommand(const std::string& deviceName, const std::string& commandName)
{
    std::string key = getKey(deviceName, commandName);
    auto it = m_commands.find(key);
    if (it != m_commands.end())
    {
        return it->second.data;
    }
    return {};
}

bool IRCommandManager::emitCommand(const std::string& deviceName, const std::string& commandName)
{
    std::cout << "[IRCmdMgr] emitCommand: " << deviceName << " -> " << commandName << std::endl;

    auto data = getCommand(deviceName, commandName);
    std::cout << "[IRCmdMgr] data size: " << data.size() << std::endl;

    if (data.empty())
    {
        std::cout << "[IRCmdMgr] Error: command data is empty" << std::endl;
        return false;
    }

    IRDevice& ir = IRDevice::getInstance();
    std::cout << "[IRCmdMgr] IRDevice isOpen: " << (ir.isOpen() ? "yes" : "no") << std::endl;

    bool result = ir.emitRawCode(data);
    std::cout << "[IRCmdMgr] emitRawCode result: " << (result ? "success" : "failed") << std::endl;

    return result;
}

std::vector<std::string> IRCommandManager::getDeviceList()
{
    std::vector<std::string> devices;
    std::unordered_map<std::string, bool> seen;

    for (auto& pair : m_commands)
    {
        std::string device = pair.second.deviceName;
        if (!seen[device])
        {
            seen[device] = true;
            devices.push_back(device);
        }
    }

    std::sort(devices.begin(), devices.end());
    return devices;
}

std::vector<std::string> IRCommandManager::getCommandList(const std::string& deviceName)
{
    std::vector<std::string> commands;

    for (auto& pair : m_commands)
    {
        if (pair.second.deviceName == deviceName)
        {
            commands.push_back(pair.second.commandName);
        }
    }

    std::sort(commands.begin(), commands.end());
    return commands;
}

bool IRCommandManager::saveToFile()
{
    std::string filePath = m_dataPath + "/ir_commands.db";
    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open())
        return false;

    uint32_t count = static_cast<uint32_t>(m_commands.size());
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto& pair : m_commands)
    {
        IRCommand& cmd = pair.second;

        uint16_t deviceNameLen = static_cast<uint16_t>(cmd.deviceName.size());
        file.write(reinterpret_cast<const char*>(&deviceNameLen), sizeof(deviceNameLen));
        file.write(cmd.deviceName.c_str(), deviceNameLen);

        uint16_t commandNameLen = static_cast<uint16_t>(cmd.commandName.size());
        file.write(reinterpret_cast<const char*>(&commandNameLen), sizeof(commandNameLen));
        file.write(cmd.commandName.c_str(), commandNameLen);

        uint32_t dataLen = static_cast<uint32_t>(cmd.data.size());
        file.write(reinterpret_cast<const char*>(&dataLen), sizeof(dataLen));
        if (dataLen > 0)
        {
            file.write(reinterpret_cast<const char*>(cmd.data.data()), dataLen);
        }

        file.write(reinterpret_cast<const char*>(&cmd.timestamp), sizeof(cmd.timestamp));
    }

    file.close();
    return true;
}

bool IRCommandManager::loadFromFile()
{
    std::string filePath = m_dataPath + "/ir_commands.db";
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open())
        return false;

    m_commands.clear();

    uint32_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; i++)
    {
        IRCommand cmd;

        uint16_t deviceNameLen;
        file.read(reinterpret_cast<char*>(&deviceNameLen), sizeof(deviceNameLen));
        cmd.deviceName.resize(deviceNameLen);
        file.read(&cmd.deviceName[0], deviceNameLen);

        uint16_t commandNameLen;
        file.read(reinterpret_cast<char*>(&commandNameLen), sizeof(commandNameLen));
        cmd.commandName.resize(commandNameLen);
        file.read(&cmd.commandName[0], commandNameLen);

        uint32_t dataLen;
        file.read(reinterpret_cast<char*>(&dataLen), sizeof(dataLen));
        cmd.data.resize(dataLen);
        if (dataLen > 0)
        {
            file.read(reinterpret_cast<char*>(cmd.data.data()), dataLen);
        }

        file.read(reinterpret_cast<char*>(&cmd.timestamp), sizeof(cmd.timestamp));

        std::string key = getKey(cmd.deviceName, cmd.commandName);
        m_commands[key] = cmd;
    }

    file.close();
    return true;
}

bool IRCommandManager::commandExists(const std::string& deviceName, const std::string& commandName)
{
    std::string key = getKey(deviceName, commandName);
    return m_commands.find(key) != m_commands.end();
}
