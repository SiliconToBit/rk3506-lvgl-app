#include "AudioDevice.h"
#include <iostream>

AudioDevice::AudioDevice() : pcm_handle(nullptr), device_name("default"), mixer_name("Master")
{
}

AudioDevice::~AudioDevice()
{
    close();
}

/**
 * @brief 打开并配置一个 ALSA PCM 播放设备。
 *
 * 此函数会执行以下操作：
 *  - 如果之前已经有打开的 PCM 句柄，则先将其关闭；
 *  - 使用当前的 device_name 以“播放”模式打开 ALSA PCM 设备；
 *  - 按照指定的采样率和声道数设置播放参数（采样格式、声道数、采样率、重采样和延迟等）。
 *
 * @param rate     采样率，单位为 Hz（例如 44100、48000）。
 * @param channels 声道数，例如 1 表示单声道，2 表示立体声。
 *
 * @return true   打开并配置成功，PCM 设备可以正常用于播放。
 * @return false  打开或配置失败，会在标准错误输出打印错误信息，并关闭已打开的句柄。
 */
bool AudioDevice::open(unsigned int rate = 44100, int channels = 2)
{
    if (pcm_handle)
    {
        close();
    }

    int err;
    if ((err = snd_pcm_open(&pcm_handle, device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        std::cerr << "Cannot open audio device " << device_name << " (" << snd_strerror(err) << ")" << std::endl;
        return false;
    }

    if ((err = snd_pcm_set_params(pcm_handle,
                                  SND_PCM_FORMAT_S16_LE, // 假设是 16位 小端
                                  SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate,
                                  1, // 允许重采样
                                  500000)) < 0)
    {
        // 0.5秒延迟
        std::cerr << "Playback open error: " << snd_strerror(err) << std::endl;
        close();
        return false;
    }

    return true;
}

void AudioDevice::close()
{
    if (pcm_handle)
    {
        snd_pcm_drain(pcm_handle); // 等待剩余数据播放完
        snd_pcm_close(pcm_handle);
        pcm_handle = nullptr;
    }
}

snd_pcm_sframes_t AudioDevice::write(const void* buffer, snd_pcm_uframes_t frames)
{
    if (!pcm_handle)
        return -1;

    snd_pcm_sframes_t frames_written = snd_pcm_writei(pcm_handle, buffer, frames);

    if (frames_written < 0)
    {
        frames_written = snd_pcm_recover(pcm_handle, frames_written, 0);
        if (frames_written < 0)
        {
            std::cerr << "snd_pcm_writei failed: " << snd_strerror(frames_written) << std::endl;
        }
    }
    return frames_written;
}

void AudioDevice::prepare()
{
    if (pcm_handle)
    {
        snd_pcm_prepare(pcm_handle);
    }
}

// --- Mixer 音量控制部分 ---

bool AudioDevice::initMixer(snd_mixer_t** handle, snd_mixer_elem_t** elem)
{
    snd_mixer_selem_id_t* sid;
    const char* card = "default";

    if (snd_mixer_open(handle, 0) < 0)
        return false;
    if (snd_mixer_attach(*handle, card) < 0)
    {
        snd_mixer_close(*handle);
        return false;
    }
    if (snd_mixer_selem_register(*handle, NULL, NULL) < 0)
    {
        snd_mixer_close(*handle);
        return false;
    }
    if (snd_mixer_load(*handle) < 0)
    {
        snd_mixer_close(*handle);
        return false;
    }

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, mixer_name.c_str());
    *elem = snd_mixer_find_selem(*handle, sid);

    if (!*elem)
    {
        snd_mixer_close(*handle);
        return false;
    }
    return true;
}

void AudioDevice::setVolume(long volume)
{
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    long min, max;

    if (initMixer(&handle, &elem))
    {
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        long vol = min + (volume * (max - min) / 100);
        snd_mixer_selem_set_playback_volume_all(elem, vol);
        snd_mixer_close(handle);
    }
}

long AudioDevice::getVolume()
{
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    long min, max, vol;
    long result = 0;

    if (initMixer(&handle, &elem))
    {
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &vol) < 0)
        {
            snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
        }
        if (max != min)
        {
            result = (vol - min) * 100 / (max - min);
        }
        snd_mixer_close(handle);
    }
    return result;
}
