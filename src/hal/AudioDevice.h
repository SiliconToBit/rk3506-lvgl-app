/**
 * @file AudioDevice.h
 * @brief 音频设备管理类
 *
 * 负责 ALSA 音频设备的打开、关闭、写入和音量控制
 * 支持 ALSA 混音器音量控制，无混音器时自动降级为软件音量
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

extern "C"
{
#include <alsa/asoundlib.h>
}

/**
 * @class AudioDevice
 * @brief ALSA 音频设备管理类
 *
 * 使用延迟初始化模式，需要调用 open() 打开设备后才能播放音频
 */
class AudioDevice
{
public:
    /**
     * @brief 构造函数
     * @param deviceName ALSA 设备名称，默认为 "default"
     */
    explicit AudioDevice(std::string_view deviceName = "default");

    /// 析构函数（自动关闭设备）
    ~AudioDevice();

    // 禁用复制操作
    AudioDevice(const AudioDevice&) = delete;
    AudioDevice& operator=(const AudioDevice&) = delete;

    // 禁用移动操作
    AudioDevice(AudioDevice&&) = delete;
    AudioDevice& operator=(AudioDevice&&) = delete;

    /**
     * @brief 打开音频设备
     * @param rate 采样率（Hz）
     * @param channels 声道数（1=单声道，2=立体声）
     * @return true 打开成功
     * @return false 打开失败
     */
    [[nodiscard]] bool openDevice(unsigned int rate, int channels);

    /// 关闭音频设备
    void closeDevice() noexcept;

    /**
     * @brief 写入音频数据
     * @param buffer 音频数据缓冲区
     * @param frames 帧数
     * @return 实际写入的帧数，失败返回负值
     */
    [[nodiscard]] snd_pcm_sframes_t write(const void* buffer, snd_pcm_uframes_t frames);

    /// 准备 PCM 设备
    void prepare() noexcept;

    /**
     * @brief 设置音量
     * @param volume 音量百分比（0-100）
     */
    void setVolume(long volume);

    /**
     * @brief 获取当前音量
     * @return 音量百分比（0-100）
     */
    [[nodiscard]] long getVolume();

    /**
     * @brief 检查设备是否已打开
     * @return true 设备已打开
     * @return false 设备未打开
     */
    [[nodiscard]] bool isOpen() const noexcept
    {
        return m_pcmHandle != nullptr;
    }

private:
    /// ALSA PCM 句柄删除器
    struct PcmHandleDeleter
    {
        void operator()(snd_pcm_t* handle) const noexcept
        {
            if (handle)
            {
                snd_pcm_drain(handle);
                snd_pcm_close(handle);
            }
        }
    };

    /// ALSA 混音器句柄删除器
    struct MixerHandleDeleter
    {
        void operator()(snd_mixer_t* handle) const noexcept
        {
            if (handle)
            {
                snd_mixer_close(handle);
            }
        }
    };

    using PcmHandlePtr = std::unique_ptr<snd_pcm_t, PcmHandleDeleter>;
    using MixerHandlePtr = std::unique_ptr<snd_mixer_t, MixerHandleDeleter>;

    std::string m_deviceName;                     ///< 设备名称
    PcmHandlePtr m_pcmHandle;                     ///< PCM 句柄
    std::atomic<long> m_volumePercent{75};        ///< 音量百分比
    std::atomic<bool> m_useSoftwareVolume{false}; ///< 是否使用软件音量
    std::vector<int16_t> m_softVolumeBuffer;      ///< 软件音量缓冲区

    /**
     * @brief 初始化混音器
     * @param handle 混音器句柄
     * @param elem 混音器元素
     * @return true 初始化成功
     * @return false 初始化失败
     */
    [[nodiscard]] bool initMixer(MixerHandlePtr& handle, snd_mixer_elem_t** elem);

    /**
     * @brief 应用软件音量
     * @param buffer 原始音频数据
     * @param frames 帧数
     * @param outBuffer 输出缓冲区指针
     */
    void applySoftwareVolume(const void* buffer, snd_pcm_uframes_t frames, const void** outBuffer);
};
