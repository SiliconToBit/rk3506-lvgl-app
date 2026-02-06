#include "MusicPlayer.h"
#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include "./ui/generated/events_init.h"
#include "./ui/generated/gui_guider.h"

MusicPlayer::MusicPlayer() : currentIndex(-1), running(false), paused(false), stopRequest(false)
{
}

MusicPlayer::~MusicPlayer()
{
    stop();
}

void MusicPlayer::scanDirectory(const std::string &dirPath)
{
    playlist.clear();
    currentIndex = -1;

    DIR *dir = opendir(dirPath.c_str());
    if (dir == nullptr)
    {
        std::cerr << "Failed to open directory: " << dirPath << std::endl;
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = entry->d_name;
        // 简单的后缀检查，支持 wav 和 mp3
        if (filename.length() > 4)
        {
            std::string ext = filename.substr(filename.length() - 4);
            // 转换为小写比较
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".wav" || ext == ".mp3")
            {
                // 存储完整路径
                std::string fullPath = dirPath;
                if (fullPath.back() != '/')
                    fullPath += "/";
                fullPath += filename;
                playlist.push_back(fullPath);
            }
        }
    }
    closedir(dir);

    // 排序播放列表
    std::sort(playlist.begin(), playlist.end());

    std::cout << "Found " << playlist.size() << " songs." << std::endl;
    if (!playlist.empty())
    {
        currentIndex = 0;
    }
}

const std::vector<std::string> &MusicPlayer::getPlaylist() const
{
    return playlist;
}

void MusicPlayer::playbackLoop(std::string filepath)
{
    if (!decoder.open(filepath.c_str()))
    {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        running = false;
        return;
    }

    // Assuming 44100Hz Stereo 16bit as per MediaDecoder implementation
    if (!audioDevice.open(44100, 2))
    {
        std::cerr << "Failed to open audio device" << std::endl;
        decoder.close();
        running = false;
        return;
    }

    std::cout << "Start playing: " << filepath << std::endl;

    while (!stopRequest)
    {
        // Handle Pause
        {
            std::unique_lock<std::mutex> lock(playMutex);
            while (paused && !stopRequest)
            {
                playCv.wait(lock);
            }
        }

        if (stopRequest)
            break;

        bool success = decoder.decode([this](uint8_t *data, int size) {
            if (stopRequest)
                return;
            // Write to ALSA
            // size is in bytes. frames = size / (channels * bytes_per_sample)
            // 16bit stereo = 4 bytes per frame
            snd_pcm_uframes_t frames = size / 4;
            snd_pcm_sframes_t written = audioDevice.write(data, frames);
            if (written < 0)
            {
                audioDevice.prepare(); // Recover from underrun
            }
        });

        if (!success)
        {
            // End of file or error
            break;
        }
    }

    decoder.close();
    audioDevice.close();
    running = false;
    std::cout << "Playback finished: " << filepath << std::endl;
}

void MusicPlayer::playFile(const std::string &filepath)
{
    stop(); // Ensure previous thread is stopped

    stopRequest = false;
    paused = false;
    running = true;

    // 解析歌词
    std::string lrcContent = getCurrentSongLyrics();
    parseLrc(lrcContent);

    // Start new thread
    playbackThread = std::thread(&MusicPlayer::playbackLoop, this, filepath);
}

void MusicPlayer::play(int index)
{
    if (index >= 0 && index < playlist.size())
    {
        currentIndex = index;
        playFile(playlist[currentIndex]);
    }
}

void MusicPlayer::play(const std::string &filename)
{
    playFile(filename);
}

void MusicPlayer::play()
{
    if (currentIndex >= 0 && currentIndex < playlist.size())
    {
        playFile(playlist[currentIndex]);
    }
}

void MusicPlayer::play(double time)
{
    decoder.seek(time);
}

void MusicPlayer::loadMusic(int index)
{
    if (index >= 0 && index < playlist.size())
    {
        currentIndex = index;
        // 停止当前播放（如果有）
        stop();

        // 仅打开解码器，不启动播放线程
        std::string filepath = playlist[currentIndex];
        if (decoder.open(filepath.c_str()))
        {
            std::cout << "Loaded music: " << filepath << std::endl;
            // 可以在这里获取封面等元数据
            // setAlbumCover(filepath);
        }
        else
        {
            std::cerr << "Failed to load music: " << filepath << std::endl;
        }
    }
}

void MusicPlayer::pause()
{
    if (running && !paused)
    {
        paused = true;
        playCv.notify_all();
    }
}

void MusicPlayer::resume()
{
    if (running && paused)
    {
        paused = false;
        playCv.notify_all();
    }
}

void MusicPlayer::stop()
{
    stopRequest = true;
    paused = false; // Break wait loop
    playCv.notify_all();

    if (playbackThread.joinable())
    {
        playbackThread.join();
    }
    running = false;
}

void MusicPlayer::next()
{
    if (playlist.empty())
        return;

    currentIndex++;
    if (currentIndex >= playlist.size())
    {
        currentIndex = 0; // 循环播放
    }

    if (isPlaying())
    {
        play(currentIndex);
    }
    else
    {
        loadMusic(currentIndex);
    }
}

void MusicPlayer::prev()
{
    if (playlist.empty())
        return;

    currentIndex--;
    if (currentIndex < 0)
    {
        currentIndex = playlist.size() - 1; // 循环播放
    }

    if (isPlaying())
    {
        play(currentIndex);
    }
    else
    {
        loadMusic(currentIndex);
    }
}

bool MusicPlayer::isPlaying() const
{
    return running && !paused;
}

std::string MusicPlayer::getCurrentSongName() const
{
    if (currentIndex >= 0 && currentIndex < playlist.size())
    {
        // 只返回文件名，不包含路径
        std::string path = playlist[currentIndex];
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos)
        {
            return path.substr(lastSlash + 1);
        }
        return path;
    }
    return "";
}

std::string MusicPlayer::getCurrentSongLyrics() const
{
    if (currentIndex >= 0 && currentIndex < playlist.size())
    {
        std::string audioPath = playlist[currentIndex];
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

std::string MusicPlayer::getCurrentAlbumCoverPath() const
{
    if (currentIndex >= 0 && currentIndex < playlist.size())
    {
        std::string audioPath = playlist[currentIndex];
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

double MusicPlayer::getMusicCurrentTime() const
{
    return decoder.getCurrentTime();
}

double MusicPlayer::getMusicDuration() const
{
    return decoder.getDuration();
}

void MusicPlayer::parseLrc(const std::string &lrcContent)
{
    currentLyrics.clear();
    std::istringstream stream(lrcContent);
    std::string line;
    while (std::getline(stream, line))
    {
        // Simple parser for [mm:ss.xx]Lyric
        size_t openBracket = line.find('[');
        size_t closeBracket = line.find(']');
        if (openBracket != std::string::npos && closeBracket != std::string::npos && closeBracket > openBracket)
        {
            std::string timeStr = line.substr(openBracket + 1, closeBracket - openBracket - 1);
            std::string text = line.substr(closeBracket + 1);

            // Parse time mm:ss.xx
            int min = 0;
            float sec = 0.0f;
            if (sscanf(timeStr.c_str(), "%d:%f", &min, &sec) >= 2)
            {
                double time = min * 60 + sec;
                // Remove \r if exists
                if (!text.empty() && text.back() == '\r')
                {
                    text.pop_back();
                }
                currentLyrics.push_back({time, text});
            }
        }
    }
    // Sort by time just in case
    std::sort(currentLyrics.begin(), currentLyrics.end(),
              [](const LyricLine &a, const LyricLine &b) { return a.time < b.time; });
}

std::string MusicPlayer::getCurrentLyricLine(double time) const
{
    if (currentLyrics.empty())
        return "";

    // Find the last lyric that has time <= current time
    // upper_bound returns first element with time > current time
    auto it = std::upper_bound(currentLyrics.begin(), currentLyrics.end(), time,
                               [](double val, const LyricLine &line) { return val < line.time; });

    if (it == currentLyrics.begin())
    {
        return ""; // Before first lyric
    }

    return std::prev(it)->text;
}
