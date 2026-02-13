#include "MusicPlayer.h"
#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

/**
 * @brief 构造函数
 * @details 初始化播放索引和状态标志
 */
MusicPlayer::MusicPlayer()
    : m_currentIndex(-1)
    , m_running(false)
    , m_paused(false)
    , m_stopRequest(false)
{
}

/**
 * @brief 析构函数
 * @details 自动停止播放
 */
MusicPlayer::~MusicPlayer()
{
    stop();
}

/**
 * @brief 扫描音乐目录
 * @param dirPath 目录路径
 * @details 扫描目录中的.wav和.mp3文件,添加到播放列表并排序
 */
void MusicPlayer::scanDirectory(const std::string& dirPath)
{
    m_playlist.clear();
    m_currentIndex = -1;

    DIR* dir = opendir(dirPath.c_str());
    if (dir == nullptr)
    {
        std::cerr << "Failed to open directory: " << dirPath << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = entry->d_name;
        if (filename.length() > 4)
        {
            std::string ext = filename.substr(filename.length() - 4);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".wav" || ext == ".mp3")
            {
                std::string fullPath = dirPath;
                if (fullPath.back() != '/')
                    fullPath += "/";
                fullPath += filename;
                m_playlist.push_back(fullPath);
            }
        }
    }
    closedir(dir);

    std::sort(m_playlist.begin(), m_playlist.end());

    std::cout << "Found " << m_playlist.size() << " songs." << std::endl;
    if (!m_playlist.empty())
    {
        m_currentIndex = 0;
    }
}

/**
 * @brief 获取播放列表
 * @return const std::vector<std::string>& 播放列表引用
 */
const std::vector<std::string>& MusicPlayer::getPlaylist() const
{
    return m_playlist;
}

/**
 * @brief 播放线程主循环
 * @param filepath 音频文件路径
 * @details 打开解码器和音频设备,循环解码播放直到停止或播放完成
 */
void MusicPlayer::playbackLoop(std::string filepath)
{
    if (!m_decoder.open(filepath.c_str()))
    {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        m_running = false;
        return;
    }

    if (!m_audioDevice.open(44100, 2))
    {
        std::cerr << "Failed to open audio device" << std::endl;
        m_decoder.close();
        m_running = false;
        return;
    }

    std::cout << "Start playing: " << filepath << std::endl;

    while (!m_stopRequest)
    {
        {
            std::unique_lock<std::mutex> lock(m_playMutex);
            while (m_paused && !m_stopRequest)
            {
                m_playCv.wait(lock);
            }
        }

        if (m_stopRequest)
            break;

        bool success = m_decoder.decode([this](uint8_t* data, int size) {
            if (m_stopRequest)
                return;
            snd_pcm_uframes_t frames = size / 4;
            snd_pcm_sframes_t written = m_audioDevice.write(data, frames);
            if (written < 0)
            {
                m_audioDevice.prepare();
            }
        });

        if (!success)
        {
            break;
        }
    }

    m_decoder.close();
    m_audioDevice.close();
    m_running = false;
    std::cout << "Playback finished: " << filepath << std::endl;
}

/**
 * @brief 播放指定文件
 * @param filepath 文件路径
 * @details 停止当前播放,加载歌词,启动播放线程
 */
void MusicPlayer::playFile(const std::string& filepath)
{
    stop();

    m_stopRequest = false;
    m_paused = false;
    m_running = true;

    std::string lrcContent = getCurrentSongLyrics();
    parseLrc(lrcContent);

    m_playbackThread = std::thread(&MusicPlayer::playbackLoop, this, filepath);
}

/**
 * @brief 播放指定索引的歌曲
 * @param index 播放列表索引
 */
void MusicPlayer::play(int index)
{
    if (index >= 0 && index < m_playlist.size())
    {
        m_currentIndex = index;
        playFile(m_playlist[m_currentIndex]);
    }
}

/**
 * @brief 播放指定文件名
 * @param filename 文件名或路径
 */
void MusicPlayer::play(const std::string& filename)
{
    playFile(filename);
}

/**
 * @brief 播放当前歌曲
 */
void MusicPlayer::play()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size())
    {
        playFile(m_playlist[m_currentIndex]);
    }
}

/**
 * @brief 从指定时间点开始播放
 * @param time 时间点(秒)
 */
void MusicPlayer::play(double time)
{
    m_decoder.seek(time);
}

/**
 * @brief 加载音乐到解码器(不播放)
 * @param index 播放列表索引
 */
void MusicPlayer::loadMusic(int index)
{
    if (index >= 0 && index < m_playlist.size())
    {
        m_currentIndex = index;
        stop();

        std::string filepath = m_playlist[m_currentIndex];
        if (m_decoder.open(filepath.c_str()))
        {
            std::cout << "Loaded music: " << filepath << std::endl;
        }
        else
        {
            std::cerr << "Failed to load music: " << filepath << std::endl;
        }
    }
}

/**
 * @brief 暂停播放
 */
void MusicPlayer::pause()
{
    if (m_running && !m_paused)
    {
        m_paused = true;
        m_playCv.notify_all();
    }
}

/**
 * @brief 恢复播放
 */
void MusicPlayer::resume()
{
    if (m_running && m_paused)
    {
        m_paused = false;
        m_playCv.notify_all();
    }
}

/**
 * @brief 停止播放
 * @details 设置停止标志并等待播放线程结束
 */
void MusicPlayer::stop()
{
    m_stopRequest = true;
    m_paused = false;
    m_playCv.notify_all();

    if (m_playbackThread.joinable())
    {
        m_playbackThread.join();
    }
    m_running = false;
}

/**
 * @brief 播放下一首
 * @details 切换到播放列表中的下一首歌曲
 */
void MusicPlayer::next()
{
    if (m_playlist.empty())
        return;

    m_currentIndex++;
    if (m_currentIndex >= m_playlist.size())
    {
        m_currentIndex = 0;
    }

    if (isPlaying())
    {
        play(m_currentIndex);
    }
    else
    {
        loadMusic(m_currentIndex);
    }
}

/**
 * @brief 播放上一首
 * @details 切换到播放列表中的上一首歌曲
 */
void MusicPlayer::prev()
{
    if (m_playlist.empty())
        return;

    m_currentIndex--;
    if (m_currentIndex < 0)
    {
        m_currentIndex = m_playlist.size() - 1;
    }

    if (isPlaying())
    {
        play(m_currentIndex);
    }
    else
    {
        loadMusic(m_currentIndex);
    }
}

/**
 * @brief 检查是否正在播放
 * @return true 正在播放
 * @return false 未播放或已暂停
 */
bool MusicPlayer::isPlaying() const
{
    return m_running && !m_paused;
}

/**
 * @brief 获取当前歌曲名称
 * @return std::string 歌曲文件名(不含路径)
 */
std::string MusicPlayer::getCurrentSongName() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size())
    {
        std::string path = m_playlist[m_currentIndex];
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos)
        {
            return path.substr(lastSlash + 1);
        }
        return path;
    }
    return "";
}

/**
 * @brief 获取当前歌曲歌词内容
 * @return std::string LRC歌词文件内容
 * @details 查找与音频文件同名的.lrc文件并读取内容
 */
std::string MusicPlayer::getCurrentSongLyrics() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size())
    {
        std::string audioPath = m_playlist[m_currentIndex];
        size_t lastDot = audioPath.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            std::string lrcPath = audioPath.substr(0, lastDot) + ".lrc";
            std::ifstream file(lrcPath);
            if (file.is_open())
            {
                std::string content;
                file.seekg(0, std::ios::end);
                content.resize(file.tellg());
                file.seekg(0, std::ios::beg);
                file.read(&content[0], content.size());
                return content;
            }
        }
    }
    return "";
}

/**
 * @brief 获取当前歌曲专辑封面路径
 * @return std::string 封面图片路径,不存在返回空字符串
 * @details 查找与音频文件同名的.jpg或.png文件
 */
std::string MusicPlayer::getCurrentAlbumCoverPath() const
{
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size())
    {
        std::string audioPath = m_playlist[m_currentIndex];
        size_t lastDot = audioPath.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            std::string basePath = audioPath.substr(0, lastDot);
            std::string jpgPath = basePath + ".jpg";
            std::string pngPath = basePath + ".png";

            struct stat buffer;
            if (stat(jpgPath.c_str(), &buffer) == 0)
                return jpgPath;
            if (stat(pngPath.c_str(), &buffer) == 0)
                return pngPath;
        }
    }
    return "";
}

/**
 * @brief 获取当前播放时间
 * @return double 当前时间(秒)
 */
double MusicPlayer::getMusicCurrentTime() const
{
    return m_decoder.getCurrentTime();
}

/**
 * @brief 获取音乐总时长
 * @return double 总时长(秒)
 */
double MusicPlayer::getMusicDuration() const
{
    return m_decoder.getDuration();
}

/**
 * @brief 设置音量
 * @param volume 音量值(0-100)
 */
void MusicPlayer::setVolume(long volume)
{
    m_audioDevice.setVolume(volume);
}

/**
 * @brief 获取当前音量
 * @return long 音量值(0-100)
 */
long MusicPlayer::getVolume()
{
    return m_audioDevice.getVolume();
}

/**
 * @brief 解析LRC歌词
 * @param lrcContent LRC格式歌词内容
 * @details 将歌词按时间戳解析并排序存储
 */
void MusicPlayer::parseLrc(const std::string& lrcContent)
{
    m_currentLyrics.clear();
    std::istringstream stream(lrcContent);
    std::string line;
    while (std::getline(stream, line))
    {
        size_t openBracket = line.find('[');
        size_t closeBracket = line.find(']');
        if (openBracket != std::string::npos && closeBracket != std::string::npos && closeBracket > openBracket)
        {
            std::string timeStr = line.substr(openBracket + 1, closeBracket - openBracket - 1);
            std::string text = line.substr(closeBracket + 1);

            int min = 0;
            float sec = 0.0f;
            if (sscanf(timeStr.c_str(), "%d:%f", &min, &sec) >= 2)
            {
                double time = min * 60 + sec;
                if (!text.empty() && text.back() == '\r')
                {
                    text.pop_back();
                }
                m_currentLyrics.push_back({time, text});
            }
        }
    }
    std::sort(m_currentLyrics.begin(), m_currentLyrics.end(),
              [](const LyricLine& a, const LyricLine& b) { return a.time < b.time; });
}

/**
 * @brief 获取指定时间对应的歌词行
 * @param time 时间点(秒)
 * @return std::string 歌词文本,无匹配返回空字符串
 */
std::string MusicPlayer::getCurrentLyricLine(double time) const
{
    if (m_currentLyrics.empty())
        return "";

    auto it = std::upper_bound(m_currentLyrics.begin(), m_currentLyrics.end(), time,
                               [](double val, const LyricLine& line) { return val < line.time; });

    if (it == m_currentLyrics.begin())
    {
        return "";
    }

    return std::prev(it)->text;
}
