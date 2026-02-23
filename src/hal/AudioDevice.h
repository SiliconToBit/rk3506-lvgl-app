/**
 * @file AudioDevice.h
 * @brief 音频设备管理类
 * @details 负责音频设备的打开、关闭、写入和音量控制
 */

#ifndef LVGL_APP_HAL_AUDIO_DEVICE_H
#define LVGL_APP_HAL_AUDIO_DEVICE_H

#include <atomic>
#include <string>
#include <vector>

extern "C"
{
#include <alsa/asoundlib.h>
}

class AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();

    bool open(unsigned int rate, int channels);
    void close();
    snd_pcm_sframes_t write(const void* buffer, snd_pcm_uframes_t frames);
    void prepare();

    void setVolume(long volume);
    long getVolume();

private:
    snd_pcm_t* m_pcmHandle;
    std::string m_deviceName;
    std::string m_mixerName;
    std::atomic<long> m_volumePercent;
    std::atomic<bool> m_useSoftwareVolume;
    std::vector<int16_t> m_softVolumeBuffer;

    bool initMixer(snd_mixer_t** handle, snd_mixer_elem_t** elem);
    void applySoftwareVolume(const void* buffer, snd_pcm_uframes_t frames, const void** outBuffer);
};

#endif // LVGL_APP_HAL_AUDIO_DEVICE_H
