/**
 * @file MediaDecoder.h
 * @brief 媒体解码器管理类
 *
 * 基于 FFmpeg 实现音频解码，支持多种音频格式
 * 使用重采样器将音频转换为统一的 16 位立体声 44100Hz 格式
 */

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

/**
 * @class MediaDecoder
 * @brief 音频媒体解码器
 *
 * 使用延迟初始化模式，需要调用 open() 打开媒体文件后才能解码
 */
class MediaDecoder
{
public:
    /**
     * @brief 媒体信息结构
     */
    struct MediaInfo
    {
        double duration;    ///< 总时长（秒）
        double currentTime; ///< 当前播放时间（秒）
        int sampleRate;     ///< 采样率
        int channels;       ///< 声道数
    };

    /// 构造函数
    MediaDecoder();

    /// 析构函数（自动释放资源）
    ~MediaDecoder();

    // 禁用复制操作
    MediaDecoder(const MediaDecoder&) = delete;
    MediaDecoder& operator=(const MediaDecoder&) = delete;

    // 禁用移动操作
    MediaDecoder(MediaDecoder&&) = delete;
    MediaDecoder& operator=(MediaDecoder&&) = delete;

    /**
     * @brief 打开媒体文件
     * @param url 文件路径
     * @return true 打开成功
     * @return false 打开失败
     */
    [[nodiscard]] bool openDevice(std::string_view url);

    /**
     * @brief 解码音频数据
     * @param callback 音频数据回调函数，参数为数据指针和数据大小
     * @return true 解码成功（可能还有更多数据）
     * @return false 解码完成或失败
     */
    [[nodiscard]] bool decode(std::function<void(uint8_t*, int)> callback);

    /// 关闭解码器
    void closeDevice() noexcept;

    /**
     * @brief 获取媒体总时长
     * @return 总时长（秒），无法获取时返回 0
     */
    [[nodiscard]] double getDuration() const noexcept;

    /**
     * @brief 获取当前播放时间
     * @return 当前时间（秒）
     */
    [[nodiscard]] double getCurrentTime() const noexcept;

    /**
     * @brief 跳转到指定时间点
     * @param seconds 目标时间（秒）
     * @return true 跳转成功
     * @return false 跳转失败
     */
    [[nodiscard]] bool seek(double seconds) noexcept;

    /**
     * @brief 检查是否已打开媒体文件
     * @return true 已打开
     * @return false 未打开
     */
    [[nodiscard]] bool isOpen() const noexcept
    {
        return m_fmtCtx != nullptr;
    }

    /**
     * @brief 获取媒体信息
     * @return 媒体信息结构
     */
    [[nodiscard]] MediaInfo getMediaInfo() const noexcept;

private:
    /// AVFormatContext 删除器
    struct FormatContextDeleter
    {
        void operator()(AVFormatContext* ctx) const noexcept
        {
            if (ctx)
            {
                avformat_close_input(&ctx);
            }
        }
    };

    /// AVCodecContext 删除器
    struct CodecContextDeleter
    {
        void operator()(AVCodecContext* ctx) const noexcept
        {
            if (ctx)
            {
                avcodec_free_context(&ctx);
            }
        }
    };

    /// SwrContext 删除器
    struct SwrContextDeleter
    {
        void operator()(SwrContext* ctx) const noexcept
        {
            if (ctx)
            {
                swr_free(&ctx);
            }
        }
    };

    /// AVPacket 删除器
    struct PacketDeleter
    {
        void operator()(AVPacket* pkt) const noexcept
        {
            if (pkt)
            {
                av_packet_free(&pkt);
            }
        }
    };

    /// AVFrame 删除器
    struct FrameDeleter
    {
        void operator()(AVFrame* frame) const noexcept
        {
            if (frame)
            {
                av_frame_free(&frame);
            }
        }
    };

    using FormatContextPtr = std::unique_ptr<AVFormatContext, FormatContextDeleter>;
    using CodecContextPtr = std::unique_ptr<AVCodecContext, CodecContextDeleter>;
    using SwrContextPtr = std::unique_ptr<SwrContext, SwrContextDeleter>;
    using PacketPtr = std::unique_ptr<AVPacket, PacketDeleter>;
    using FramePtr = std::unique_ptr<AVFrame, FrameDeleter>;

    FormatContextPtr m_fmtCtx;  ///< 格式上下文
    CodecContextPtr m_decCtx;   ///< 解码器上下文
    SwrContextPtr m_swrCtx;     ///< 重采样上下文
    PacketPtr m_packet;         ///< 数据包
    FramePtr m_frame;           ///< 解码帧
    int m_audioStreamIndex{-1}; ///< 音频流索引
    double m_currentTime{0.0};  ///< 当前播放时间

    // 重采样输出缓冲区
    uint8_t** m_dstData{nullptr}; ///< 输出数据指针数组
    int m_dstLinesize{0};         ///< 输出行大小
    int m_maxDstNbSamples{0};     ///< 最大输出采样数

    /// 释放重采样输出缓冲区
    void freeDstBuffer() noexcept;
};
