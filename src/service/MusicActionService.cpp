#include "MusicActionService.h"
#include "MusicPlayer.h"
#include <iostream>

const char *MusicActionService::kDefaultMusicDir = "/root/Music";

MusicActionService &MusicActionService::getInstance()
{
    static MusicActionService instance;
    return instance;
}

void MusicActionService::initScreen()
{
    std::cout << "[MusicActionService] Init music screen" << std::endl;
    loadDirectory();

    MusicPlayer &player = MusicPlayer::getInstance();

    if (!player.getPlaylist().empty())
    {
        player.loadMusic(0);

        const std::string songName = player.getCurrentSongName();
        const std::string lyrics = player.getCurrentSongLyrics();
        const std::string coverPath = player.getCurrentAlbumCoverPath();

        std::cout << "[MusicActionService] First song: " << songName << std::endl;
        std::cout << "[MusicActionService] Lyrics loaded: " << (lyrics.empty() ? "no" : "yes") << std::endl;
        std::cout << "[MusicActionService] Album cover: "
                  << (coverPath.empty() ? "none" : coverPath) << std::endl;
    }
}

void MusicActionService::deinitScreen()
{
    std::cout << "[MusicActionService] Deinit music screen" << std::endl;
    MusicPlayer::getInstance().stop();
}

void MusicActionService::loadDirectory(const std::string &path)
{
    const std::string targetPath = path.empty() ? kDefaultMusicDir : path;
    std::cout << "[MusicActionService] Loading music directory: " << targetPath << std::endl;
    MusicPlayer::getInstance().scanDirectory(targetPath);
}

void MusicActionService::togglePlayPause(bool checked)
{
    std::cout << "[MusicActionService] Toggle play/pause: " << (checked ? "play" : "pause") << std::endl;
    if (checked)
    {
        MusicPlayer::getInstance().play();
        return;
    }
    MusicPlayer::getInstance().pause();
}

void MusicActionService::playNext()
{
    std::cout << "[MusicActionService] Play next" << std::endl;
    MusicPlayer::getInstance().next();
}

void MusicActionService::playPrev()
{
    std::cout << "[MusicActionService] Play previous" << std::endl;
    MusicPlayer::getInstance().prev();
}

std::string MusicActionService::getCurrentSongName() const
{
    std::string songName = MusicPlayer::getInstance().getCurrentSongName();
    const std::size_t lastDot = songName.find_last_of('.');
    if (lastDot != std::string::npos)
    {
        songName = songName.substr(0, lastDot);
    }
    return songName;
}

std::string MusicActionService::getCurrentSongLyrics() const
{
    return MusicPlayer::getInstance().getCurrentSongLyrics();
}

std::string MusicActionService::getCurrentLyricLine() const
{
    MusicPlayer &player = MusicPlayer::getInstance();
    return player.getCurrentLyricLine(player.getMusicCurrentTime());
}

std::string MusicActionService::getCurrentAlbumCoverPath() const
{
    return MusicPlayer::getInstance().getCurrentAlbumCoverPath();
}

double MusicActionService::getCurrentTime() const
{
    return MusicPlayer::getInstance().getMusicCurrentTime();
}

double MusicActionService::getTotalDuration() const
{
    return MusicPlayer::getInstance().getMusicDuration();
}

void MusicActionService::setVolume(long volume)
{
    if (volume < 0)
    {
        volume = 0;
    }
    else if (volume > 100)
    {
        volume = 100;
    }
    MusicPlayer::getInstance().setVolume(volume);
}

long MusicActionService::getVolume() const
{
    return MusicPlayer::getInstance().getVolume();
}

void MusicActionService::seekTo(double seconds)
{
    if (seconds < 0)
    {
        seconds = 0;
    }
    MusicPlayer::getInstance().play(seconds);
}

std::vector<std::string> MusicActionService::getPlaylist() const
{
    const auto& playlist = MusicPlayer::getInstance().getPlaylist();
    std::vector<std::string> result;
    for (const auto& path : playlist)
    {
        // 从完整路径中提取文件名（不含扩展名）
        std::string filename = path;
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != std::string::npos)
        {
            filename = filename.substr(lastSlash + 1);
        }
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            filename = filename.substr(0, lastDot);
        }
        result.push_back(filename);
    }
    return result;
}

void MusicActionService::playAtIndex(int index)
{
    MusicPlayer::getInstance().play(index);
}
