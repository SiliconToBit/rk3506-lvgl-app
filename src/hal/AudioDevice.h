#ifndef AUDIO_DEVICE_HPP
#define AUDIO_DEVICE_HPP


#include <string>

extern "C" {
#include <alsa/asoundlib.h>
}


class AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();

    // 打开音频设备 (PCM)
    // rate: 采样率 (如 44100)
    // channels: 通道数 (如 2)
    bool open(unsigned int rate, int channels);

    // 关闭设备
    void close();

    // 写入 PCM 数据进行播放
    // 返回实际写入的帧数，<0 表示错误
    snd_pcm_sframes_t write(const void* buffer, snd_pcm_uframes_t frames);

    // 准备设备（用于从 underrun 恢复等）
    void prepare();

    // 音量控制 (0-100)
    void setVolume(long volume);
    long getVolume();

private:
    snd_pcm_t* pcm_handle;
    std::string device_name;
    std::string mixer_name;

    bool initMixer(snd_mixer_t** handle, snd_mixer_elem_t** elem);
};

#endif // AUDIO_DEVICE_HPP
