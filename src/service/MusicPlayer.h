#ifndef LVGL_APP_SERVICE_MUSIC_PLAYER_H
#define LVGL_APP_SERVICE_MUSIC_PLAYER_H

#include "AudioDevice.h"
#include "MediaDecoder.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();

    void scanDirectory(const std::string& dirPath);
    const std::vector<std::string>& getPlaylist() const;

    void play(int index);
    void play(const std::string& filename);
    void play();
    void play(double time);

    void loadMusic(int index);
    void pause();
    void resume();
    void stop();
    void next();
    void prev();

    bool isPlaying() const;
    std::string getCurrentSongName() const;
    std::string getCurrentSongLyrics() const;
    std::string getCurrentAlbumCoverPath() const;
    std::string getCurrentLyricLine(double time) const;

    void setVolume(long volume);
    long getVolume();

    double getMusicDuration() const;
    double getMusicCurrentTime() const;

private:
    struct LyricLine
    {
        double time;
        std::string text;
    };

    std::vector<LyricLine> m_currentLyrics;
    std::vector<std::string> m_playlist;
    int m_currentIndex;
    std::string m_currentSongLyrics;

    std::thread m_playbackThread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;
    std::atomic<bool> m_stopRequest;
    std::mutex m_playMutex;
    std::condition_variable m_playCv;

    AudioDevice m_audioDevice;
    MediaDecoder m_decoder;

    void parseLrc(const std::string& lrcContent);
    void playbackLoop(std::string filepath);
    void playFile(const std::string& filepath);
};

#endif // LVGL_APP_SERVICE_MUSIC_PLAYER_H
