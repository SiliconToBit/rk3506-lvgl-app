#include "MusicPlayer.h"
#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

MusicPlayer::MusicPlayer()
    : m_currentIndex(-1)
    , m_running(false)
    , m_paused(false)
    , m_stopRequest(false)
{
}

MusicPlayer::~MusicPlayer()
{
    stop();
}

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

const std::vector<std::string>& MusicPlayer::getPlaylist() const
{
    return m_playlist;
}

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

void MusicPlayer::play(int index)
{
    if (index >= 0 && index < m_playlist.size())
    {
        m_currentIndex = index;
        playFile(m_playlist[m_currentIndex]);
    }
}

void MusicPlayer::play(const std::string& filename)
{
    playFile(filename);
}

void MusicPlayer::play()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_playlist.size())
    {
        playFile(m_playlist[m_currentIndex]);
    }
}

void MusicPlayer::play(double time)
{
    m_decoder.seek(time);
}

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

void MusicPlayer::pause()
{
    if (m_running && !m_paused)
    {
        m_paused = true;
        m_playCv.notify_all();
    }
}

void MusicPlayer::resume()
{
    if (m_running && m_paused)
    {
        m_paused = false;
        m_playCv.notify_all();
    }
}

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

bool MusicPlayer::isPlaying() const
{
    return m_running && !m_paused;
}

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

double MusicPlayer::getMusicCurrentTime() const
{
    return m_decoder.getCurrentTime();
}

double MusicPlayer::getMusicDuration() const
{
    return m_decoder.getDuration();
}

void MusicPlayer::setVolume(long volume)
{
    m_audioDevice.setVolume(volume);
}

long MusicPlayer::getVolume()
{
    return m_audioDevice.getVolume();
}

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
