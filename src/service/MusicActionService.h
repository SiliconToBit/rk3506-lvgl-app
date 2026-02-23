/**
 * @file MusicActionService.h
 * @brief 音乐动作应用服务层
 * @details 封装 UI 动作语义，编排 MusicPlayer 调用
 */

#ifndef LVGL_APP_SERVICE_MUSIC_ACTION_SERVICE_H
#define LVGL_APP_SERVICE_MUSIC_ACTION_SERVICE_H

#include <string>
#include <vector>

class MusicActionService
{
public:
    static MusicActionService &getInstance();

    MusicActionService(const MusicActionService &) = delete;
    MusicActionService &operator=(const MusicActionService &) = delete;

    void initScreen();
    void deinitScreen();
    void loadDirectory(const std::string &path = "");
    void togglePlayPause(bool checked);
    void playNext();
    void playPrev();

    std::string getCurrentSongName() const;
    std::string getCurrentSongLyrics() const;
    std::string getCurrentLyricLine() const;
    std::string getCurrentAlbumCoverPath() const;
    double getCurrentTime() const;
    double getTotalDuration() const;
    void setVolume(long volume);
    long getVolume() const;
    void seekTo(double seconds);
    std::vector<std::string> getPlaylist() const;
    void playAtIndex(int index);

private:
    MusicActionService() = default;
    ~MusicActionService() = default;

    static const char *kDefaultMusicDir;
};

#endif // LVGL_APP_SERVICE_MUSIC_ACTION_SERVICE_H
