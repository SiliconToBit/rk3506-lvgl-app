#ifndef MUSIC_PLAYER_HPP
#define MUSIC_PLAYER_HPP

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

    // 扫描指定目录下的音乐文件
    void scanDirectory(const std::string &dirPath);

    const std::vector<std::string> &getPlaylist() const;

    void play(int index);

    void play(const std::string &filename); // 保留旧接口兼容
    void play();                            // 播放当前索引
    void play(double time);                 // 从指定时间点播放

    // 预加载音乐（只打开文件，不播放）
    void loadMusic(int index);

    void pause();

    void resume();

    void stop();

    void next();

    void prev();

    bool isPlaying() const;

    std::string getCurrentSongName() const;

    std::string getCurrentSongLyrics() const;

    // 获取当前播放歌曲的封面路径
    std::string getCurrentAlbumCoverPath() const;

    // 获取当前播放时间对应的单行歌词
    std::string getCurrentLyricLine(double time) const;

    // 音量控制 (0-100)
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
    std::vector<LyricLine> currentLyrics;
    void parseLrc(const std::string &lrcContent);

    void playbackLoop(std::string filepath);
    void playFile(const std::string &filepath);

    std::vector<std::string> playlist;
    int currentIndex;
    std::string currentSongLyrics;

    // 播放控制
    std::thread playbackThread;
    std::atomic<bool> running;     // 线程运行标志
    std::atomic<bool> paused;      // 暂停标志
    std::atomic<bool> stopRequest; // 停止请求
    std::mutex playMutex;
    std::condition_variable playCv;

    // 硬件/解码组件
    AudioDevice audioDevice;
    MediaDecoder decoder;
};

#endif // MUSIC_PLAYER_HPP
