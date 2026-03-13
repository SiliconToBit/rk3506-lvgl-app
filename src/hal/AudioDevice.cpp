#include "AudioDevice.h"
#include <algorithm>
#include <cstdint>
#include <iostream>

/**
 * @brief 构造函数
 * @details 初始化PCM句柄和设备名称为默认值
 */
AudioDevice::AudioDevice()
    : m_pcmHandle(nullptr), m_deviceName("default"), m_mixerName("Master"),
      m_volumePercent(75) // 默认音量 75%，对应 DAC VOLUME 180
      ,
      m_useSoftwareVolume(false)
{
}

/**
 * @brief 析构函数
 * @details 自动关闭音频设备
 */
AudioDevice::~AudioDevice()
{
    close();
}

/**
 * @brief 打开音频设备
 * @param rate 采样率(Hz)
 * @param channels 声道数(1=单声道, 2=立体声)
 * @return true 打开成功
 * @return false 打开失败
 * @details 配置PCM设备参数:16位小端格式,交错访问模式
 */
bool AudioDevice::open(unsigned int rate, int channels)
{
    if (m_pcmHandle)
    {
        close();
    }

    int err;
    if ((err = snd_pcm_open(&m_pcmHandle, m_deviceName.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        std::cerr << "Cannot open audio device " << m_deviceName << " (" << snd_strerror(err) << ")" << std::endl;
        return false;
    }

    if ((err = snd_pcm_set_params(m_pcmHandle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 1,
                                  500000)) < 0)
    {
        std::cerr << "Playback open error: " << snd_strerror(err) << std::endl;
        close();
        return false;
    }

    return true;
}

/**
 * @brief 关闭音频设备
 * @details 排空缓冲区并关闭PCM设备
 */
void AudioDevice::close()
{
    if (m_pcmHandle)
    {
        snd_pcm_drain(m_pcmHandle);
        snd_pcm_close(m_pcmHandle);
        m_pcmHandle = nullptr;
    }
}

/**
 * @brief 写入音频数据
 * @param buffer 音频数据缓冲区
 * @param frames 帧数
 * @return snd_pcm_sframes_t 实际写入的帧数,失败返回负值
 * @details 写入PCM数据,出错时自动恢复
 */
snd_pcm_sframes_t AudioDevice::write(const void* buffer, snd_pcm_uframes_t frames)
{
    if (!m_pcmHandle)
        return -1;

    const void* writeBuffer = buffer;
    applySoftwareVolume(buffer, frames, &writeBuffer);

    snd_pcm_sframes_t frames_written = snd_pcm_writei(m_pcmHandle, writeBuffer, frames);

    if (frames_written < 0)
    {
        frames_written = snd_pcm_recover(m_pcmHandle, frames_written, 0);
        if (frames_written < 0)
        {
            std::cerr << "snd_pcm_writei failed: " << snd_strerror(frames_written) << std::endl;
        }
    }
    return frames_written;
}

/**
 * @brief 准备PCM设备
 * @details 将PCM设备置于准备状态
 */
void AudioDevice::prepare()
{
    if (m_pcmHandle)
    {
        snd_pcm_prepare(m_pcmHandle);
    }
}

/**
 * @brief 初始化混音器
 * @param handle 输出参数,混音器句柄
 * @param elem 输出参数,混音器元素
 * @return true 初始化成功
 * @return false 初始化失败
 * @details 打开混音器并查找指定元素
 */
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

    static const char* controls[] = {"DAC VOLUME", "Master", "PCM", "Speaker", "Headphone"};

    *elem = nullptr;
    for (const char* controlName : controls)
    {
        snd_mixer_selem_id_set_name(sid, controlName);
        *elem = snd_mixer_find_selem(*handle, sid);
        if (*elem)
        {
            m_mixerName = controlName;
            break;
        }
    }

    if (!*elem)
    {
        std::cerr << "[AudioDevice] No ALSA mixer playback control found, fallback to software volume" << std::endl;
        snd_mixer_close(*handle);
        return false;
    }
    return true;
}

/**
 * @brief 设置音量
 * @param volume 音量百分比(0-100)
 * @details 通过ALSA混音器设置主音量
 */
void AudioDevice::setVolume(long volume)
{
    volume = std::clamp(volume, 0L, 100L);
    m_volumePercent.store(volume, std::memory_order_relaxed);

    std::cout << "[AudioDevice] setVolume request=" << volume << "%" << std::endl;

    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    long min, max;

    if (initMixer(&handle, &elem))
    {
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        long vol;
        if (m_mixerName == "DAC VOLUME")
        {
            // DAC VOLUME: 硬件范围 0-255，但有效范围 0-240
            // 将 0-100% 映射到 0-240
            const long effectiveMax = 240;
            vol = volume * effectiveMax / 100;
        }
        else
        {
            // 其他混音器使用标准映射
            vol = min + (volume * (max - min) / 100);
        }

        snd_mixer_selem_set_playback_volume_all(elem, vol);

        snd_mixer_close(handle);
        m_useSoftwareVolume.store(false, std::memory_order_relaxed);
        std::cout << "[AudioDevice] setVolume applied via ALSA mixer: " << m_mixerName << " (" << min << "-" << max
                  << ") -> " << vol << std::endl;
    }
    else
    {
        m_useSoftwareVolume.store(true, std::memory_order_relaxed);
        std::cout << "[AudioDevice] setVolume fallback to software scaling" << std::endl;
    }
}

/**
 * @brief 获取当前音量
 * @return long 音量百分比(0-100)
 * @details 通过ALSA混音器读取主音量
 */
long AudioDevice::getVolume()
{
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    long min, max, vol;
    long result = m_volumePercent.load(std::memory_order_relaxed);

    if (initMixer(&handle, &elem))
    {
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &vol) < 0)
        {
            snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
        }

        if (m_mixerName == "DAC VOLUME")
        {
            // DAC VOLUME: 从 0-240 映射回 0-100%
            const long effectiveMax = 240;
            if (vol > effectiveMax)
                vol = effectiveMax;
            result = vol * 100 / effectiveMax;
        }
        else if (max != min)
        {
            result = (vol - min) * 100 / (max - min);
        }

        snd_mixer_close(handle);
        m_volumePercent.store(result, std::memory_order_relaxed);
        m_useSoftwareVolume.store(false, std::memory_order_relaxed);
    }
    else
    {
        m_useSoftwareVolume.store(true, std::memory_order_relaxed);
    }
    return result;
}

void AudioDevice::applySoftwareVolume(const void* buffer, snd_pcm_uframes_t frames, const void** outBuffer)
{
    if (!outBuffer)
    {
        return;
    }

    const long volumePercent = m_volumePercent.load(std::memory_order_relaxed);
    if (volumePercent >= 100)
    {
        *outBuffer = buffer;
        return;
    }

    const int16_t* src = static_cast<const int16_t*>(buffer);
    const std::size_t sampleCount = static_cast<std::size_t>(frames) * 2;
    m_softVolumeBuffer.resize(sampleCount);

    const int scale = static_cast<int>((volumePercent * volumePercent) / 100);
    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        int32_t scaled = static_cast<int32_t>(src[i]) * scale / 100;
        if (scaled > INT16_MAX)
        {
            scaled = INT16_MAX;
        }
        else if (scaled < INT16_MIN)
        {
            scaled = INT16_MIN;
        }
        m_softVolumeBuffer[i] = static_cast<int16_t>(scaled);
    }

    *outBuffer = m_softVolumeBuffer.data();
}
