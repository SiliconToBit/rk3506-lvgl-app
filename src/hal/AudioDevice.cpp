/**
 * @file AudioDevice.cpp
 * @brief 音频设备管理类实现
 *
 * 使用 ALSA 库实现音频播放和音量控制
 */

#include "AudioDevice.h"

#include <algorithm>
#include <iostream>

/// DAC VOLUME 有效最大值
constexpr long DAC_VOLUME_MAX = 240;

/// 默认音量百分比
constexpr long DEFAULT_VOLUME = 75;

/**
 * @brief 获取单例实例
 * @return AudioDevice& 单例引用
 *
 * 使用静态局部变量实现线程安全的单例模式（Meyer's Singleton）
 */
AudioDevice& AudioDevice::getInstance()
{
    static AudioDevice instance;
    return instance;
}

/**
 * @brief 构造函数
 *
 * 使用默认设备名称 "default"
 */
AudioDevice::AudioDevice()
    : m_deviceName{"default"}
    , m_volumePercent{DEFAULT_VOLUME}
{
}

/**
 * @brief 析构函数
 *
 * 由 PcmHandlePtr 自动关闭设备
 */
AudioDevice::~AudioDevice() = default;

/**
 * @brief 打开音频设备
 * @param rate 采样率（Hz）
 * @param channels 声道数（1=单声道，2=立体声）
 * @return true 打开成功
 * @return false 打开失败
 *
 * 配置 PCM 设备参数：16 位小端格式，交错访问模式
 */
bool AudioDevice::openDevice(unsigned int rate, int channels)
{
    // 如果设备已打开，先关闭
    if (m_pcmHandle)
    {
        m_pcmHandle.reset();
    }

    snd_pcm_t* handle = nullptr;
    int err = snd_pcm_open(&handle, m_deviceName.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
    {
        std::cerr << "[AudioDevice] Cannot open audio device " << m_deviceName << " (" << snd_strerror(err) << ")\n";
        return false;
    }

    // 配置 PCM 参数
    err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate, 1, 500000);
    if (err < 0)
    {
        std::cerr << "[AudioDevice] Playback open error: " << snd_strerror(err) << '\n';
        return false;
    }

    m_pcmHandle.reset(handle);
    return true;
}

/**
 * @brief 关闭音频设备
 *
 * 由 PcmHandlePtr 自动调用，排空缓冲区并关闭 PCM 设备
 */
void AudioDevice::closeDevice() noexcept
{
    m_pcmHandle.reset();
}

/**
 * @brief 写入音频数据
 * @param buffer 音频数据缓冲区
 * @param frames 帧数
 * @return 实际写入的帧数，失败返回负值
 *
 * 写入 PCM 数据，出错时自动恢复
 */
snd_pcm_sframes_t AudioDevice::write(const void* buffer, snd_pcm_uframes_t frames)
{
    if (!m_pcmHandle)
    {
        return -1;
    }

    const void* writeBuffer = buffer;
    applySoftwareVolume(buffer, frames, &writeBuffer);

    snd_pcm_sframes_t framesWritten = snd_pcm_writei(m_pcmHandle.get(), writeBuffer, frames);

    if (framesWritten < 0)
    {
        framesWritten = snd_pcm_recover(m_pcmHandle.get(), framesWritten, 0);
        if (framesWritten < 0)
        {
            std::cerr << "[AudioDevice] snd_pcm_writei failed: " << snd_strerror(framesWritten) << '\n';
        }
    }

    return framesWritten;
}

/**
 * @brief 准备 PCM 设备
 *
 * 将 PCM 设备置于准备状态
 */
void AudioDevice::prepare() noexcept
{
    if (m_pcmHandle)
    {
        snd_pcm_prepare(m_pcmHandle.get());
    }
}

/**
 * @brief 初始化混音器
 * @param handle 混音器句柄（输出）
 * @param elem 混音器元素（输出）
 * @return true 初始化成功
 * @return false 初始化失败
 *
 * 打开混音器并查找可用的音量控制元素
 */
bool AudioDevice::initMixer(MixerHandlePtr& handle, snd_mixer_elem_t** elem)
{
    snd_mixer_t* mixerHandle = nullptr;
    snd_mixer_selem_id_t* sid = nullptr;

    if (snd_mixer_open(&mixerHandle, 0) < 0)
    {
        return false;
    }

    if (snd_mixer_attach(mixerHandle, "default") < 0)
    {
        snd_mixer_close(mixerHandle);
        return false;
    }

    if (snd_mixer_selem_register(mixerHandle, nullptr, nullptr) < 0)
    {
        snd_mixer_close(mixerHandle);
        return false;
    }

    if (snd_mixer_load(mixerHandle) < 0)
    {
        snd_mixer_close(mixerHandle);
        return false;
    }

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);

    // 尝试查找可用的音量控制
    static constexpr const char* controls[] = {"DAC VOLUME", "Master", "PCM", "Speaker", "Headphone"};

    *elem = nullptr;
    for (const char* controlName : controls)
    {
        snd_mixer_selem_id_set_name(sid, controlName);
        *elem = snd_mixer_find_selem(mixerHandle, sid);
        if (*elem)
        {
            break;
        }
    }

    if (!*elem)
    {
        std::cerr << "[AudioDevice] No ALSA mixer playback control found, fallback to software volume\n";
        snd_mixer_close(mixerHandle);
        return false;
    }

    handle.reset(mixerHandle);
    return true;
}

/**
 * @brief 设置音量
 * @param volume 音量百分比（0-100）
 *
 * 通过 ALSA 混音器设置主音量，无混音器时使用软件音量
 */
void AudioDevice::setVolume(long volume)
{
    volume = std::clamp(volume, 0L, 100L);
    m_volumePercent.store(volume, std::memory_order_relaxed);

    std::cout << "[AudioDevice] setVolume request=" << volume << "%\n";

    MixerHandlePtr mixerHandle;
    snd_mixer_elem_t* elem = nullptr;

    if (initMixer(mixerHandle, &elem))
    {
        long min = 0;
        long max = 0;
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        long vol;

        // 判断是否为 DAC VOLUME 控制
        if (max == 255 || max == 240)
        {
            // DAC VOLUME: 有效范围 0-240
            vol = volume * DAC_VOLUME_MAX / 100;
        }
        else
        {
            // 其他混音器使用标准映射
            vol = min + (volume * (max - min) / 100);
        }

        snd_mixer_selem_set_playback_volume_all(elem, vol);

        m_useSoftwareVolume.store(false, std::memory_order_relaxed);
        std::cout << "[AudioDevice] setVolume applied via ALSA mixer (" << min << "-" << max << ") -> " << vol << '\n';
    }
    else
    {
        m_useSoftwareVolume.store(true, std::memory_order_relaxed);
        std::cout << "[AudioDevice] setVolume fallback to software scaling\n";
    }
}

/**
 * @brief 获取当前音量
 * @return 音量百分比（0-100）
 *
 * 通过 ALSA 混音器读取主音量
 */
long AudioDevice::getVolume()
{
    MixerHandlePtr mixerHandle;
    snd_mixer_elem_t* elem = nullptr;
    long result = m_volumePercent.load(std::memory_order_relaxed);

    if (initMixer(mixerHandle, &elem))
    {
        long min = 0;
        long max = 0;
        long vol = 0;

        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

        if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &vol) < 0)
        {
            snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
        }

        // 判断是否为 DAC VOLUME 控制
        if (max == 255 || max == 240)
        {
            // DAC VOLUME: 从 0-240 映射回 0-100%
            vol = std::min(vol, DAC_VOLUME_MAX);
            result = vol * 100 / DAC_VOLUME_MAX;
        }
        else if (max != min)
        {
            result = (vol - min) * 100 / (max - min);
        }

        m_volumePercent.store(result, std::memory_order_relaxed);
        m_useSoftwareVolume.store(false, std::memory_order_relaxed);
    }
    else
    {
        m_useSoftwareVolume.store(true, std::memory_order_relaxed);
    }

    return result;
}

/**
 * @brief 应用软件音量
 * @param buffer 原始音频数据
 * @param frames 帧数
 * @param outBuffer 输出缓冲区指针
 *
 * 当硬件音量控制不可用时，通过软件缩放实现音量调节
 */
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

    // 使用二次曲线实现更自然的音量调节
    const int scale = static_cast<int>((volumePercent * volumePercent) / 100);

    for (std::size_t i = 0; i < sampleCount; ++i)
    {
        int32_t scaled = static_cast<int32_t>(src[i]) * scale / 100;
        scaled = std::clamp(scaled, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX));
        m_softVolumeBuffer[i] = static_cast<int16_t>(scaled);
    }

    *outBuffer = m_softVolumeBuffer.data();
}
