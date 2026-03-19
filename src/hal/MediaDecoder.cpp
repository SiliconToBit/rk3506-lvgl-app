/**
 * @file MediaDecoder.cpp
 * @brief 媒体解码器管理类实现
 *
 * 使用 FFmpeg 库实现音频解码和重采样
 */

#include "MediaDecoder.h"

#include <iostream>

/// 输出采样率
constexpr int OUTPUT_SAMPLE_RATE = 44100;

/// 输出声道数
constexpr int OUTPUT_CHANNELS = 2;

/// 输出采样格式
constexpr AVSampleFormat OUTPUT_SAMPLE_FMT = AV_SAMPLE_FMT_S16;

/**
 * @brief 构造函数
 *
 * 分配 AVPacket 和 AVFrame
 */
MediaDecoder::MediaDecoder()
    : m_packet{av_packet_alloc()}
    , m_frame{av_frame_alloc()}
{
}

/**
 * @brief 析构函数
 *
 * 由智能指针自动释放所有资源
 */
MediaDecoder::~MediaDecoder()
{
    closeDevice();
}

/**
 * @brief 打开媒体文件
 * @param url 文件路径
 * @return true 打开成功
 * @return false 打开失败
 *
 * 打开文件，查找音频流，初始化解码器和重采样器
 */
bool MediaDecoder::openDevice(std::string_view url)
{
    // 如果已打开，先关闭
    closeDevice();

    // 打开输入文件
    AVFormatContext* fmtCtx = avformat_alloc_context();
    if (avformat_open_input(&fmtCtx, url.data(), nullptr, nullptr) < 0)
    {
        std::cerr << "[MediaDecoder] Could not open source file " << url << '\n';
        return false;
    }
    m_fmtCtx.reset(fmtCtx);

    // 获取流信息
    if (avformat_find_stream_info(m_fmtCtx.get(), nullptr) < 0)
    {
        std::cerr << "[MediaDecoder] Could not find stream information\n";
        m_fmtCtx.reset();
        return false;
    }

    // 查找音频流
    m_audioStreamIndex = av_find_best_stream(m_fmtCtx.get(), AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (m_audioStreamIndex < 0)
    {
        std::cerr << "[MediaDecoder] Could not find audio stream in input file\n";
        m_fmtCtx.reset();
        return false;
    }

    AVStream* audioStream = m_fmtCtx->streams[m_audioStreamIndex];
    const AVCodec* decoder = avcodec_find_decoder(audioStream->codecpar->codec_id);
    if (!decoder)
    {
        std::cerr << "[MediaDecoder] Failed to find decoder codec\n";
        m_fmtCtx.reset();
        return false;
    }

    // 分配解码器上下文
    AVCodecContext* decCtx = avcodec_alloc_context3(decoder);
    if (!decCtx)
    {
        std::cerr << "[MediaDecoder] Failed to allocate codec context\n";
        m_fmtCtx.reset();
        return false;
    }
    m_decCtx.reset(decCtx);

    // 复制编解码器参数
    if (avcodec_parameters_to_context(m_decCtx.get(), audioStream->codecpar) < 0)
    {
        std::cerr << "[MediaDecoder] Failed to copy codec parameters to decoder context\n";
        m_decCtx.reset();
        m_fmtCtx.reset();
        return false;
    }

    // 打开解码器
    if (avcodec_open2(m_decCtx.get(), decoder, nullptr) < 0)
    {
        std::cerr << "[MediaDecoder] Failed to open codec\n";
        m_decCtx.reset();
        m_fmtCtx.reset();
        return false;
    }

    // 初始化重采样器
    SwrContext* swrCtx = swr_alloc();
    if (!swrCtx)
    {
        std::cerr << "[MediaDecoder] Could not allocate resampler context\n";
        m_decCtx.reset();
        m_fmtCtx.reset();
        return false;
    }
    m_swrCtx.reset(swrCtx);

    // 配置重采样器输入参数
    av_opt_set_int(
        m_swrCtx.get(), "in_channel_layout",
        m_decCtx->channel_layout ? m_decCtx->channel_layout : av_get_default_channel_layout(m_decCtx->channels), 0);
    av_opt_set_int(m_swrCtx.get(), "in_sample_rate", m_decCtx->sample_rate, 0);
    av_opt_set_sample_fmt(m_swrCtx.get(), "in_sample_fmt", m_decCtx->sample_fmt, 0);

    // 配置重采样器输出参数
    av_opt_set_int(m_swrCtx.get(), "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(m_swrCtx.get(), "out_sample_rate", OUTPUT_SAMPLE_RATE, 0);
    av_opt_set_sample_fmt(m_swrCtx.get(), "out_sample_fmt", OUTPUT_SAMPLE_FMT, 0);

    // 初始化重采样器
    if (swr_init(m_swrCtx.get()) < 0)
    {
        std::cerr << "[MediaDecoder] Failed to initialize the resampling context\n";
        m_swrCtx.reset();
        m_decCtx.reset();
        m_fmtCtx.reset();
        return false;
    }

    return true;
}

/**
 * @brief 解码音频数据
 * @param callback 音频数据回调函数
 * @return true 解码成功（可能还有更多数据）
 * @return false 解码完成或失败
 *
 * 读取一帧数据，解码并通过重采样转换为 16 位立体声 44100Hz 格式
 */
bool MediaDecoder::decode(std::function<void(uint8_t*, int)> callback)
{
    if (!m_fmtCtx || !m_decCtx)
    {
        return false;
    }

    int ret = av_read_frame(m_fmtCtx.get(), m_packet.get());
    if (ret < 0)
    {
        return false;
    }

    if (m_packet->stream_index == m_audioStreamIndex)
    {
        ret = avcodec_send_packet(m_decCtx.get(), m_packet.get());
        if (ret < 0)
        {
            std::cerr << "[MediaDecoder] Error submitting the packet to the decoder\n";
            av_packet_unref(m_packet.get());
            return false;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_frame(m_decCtx.get(), m_frame.get());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            {
                break;
            }
            else if (ret < 0)
            {
                std::cerr << "[MediaDecoder] Error during decoding\n";
                av_packet_unref(m_packet.get());
                return false;
            }

            // 更新当前播放时间
            if (m_frame->pts != AV_NOPTS_VALUE)
            {
                AVStream* stream = m_fmtCtx->streams[m_audioStreamIndex];
                m_currentTime = m_frame->pts * av_q2d(stream->time_base);
            }

            // 计算输出采样数
            int dstNbSamples =
                av_rescale_rnd(swr_get_delay(m_swrCtx.get(), m_decCtx->sample_rate) + m_frame->nb_samples,
                               OUTPUT_SAMPLE_RATE, m_decCtx->sample_rate, AV_ROUND_UP);

            // 分配或扩展输出缓冲区
            if (dstNbSamples > m_maxDstNbSamples)
            {
                freeDstBuffer();
                av_samples_alloc_array_and_samples(&m_dstData, &m_dstLinesize, OUTPUT_CHANNELS, dstNbSamples,
                                                   OUTPUT_SAMPLE_FMT, 0);
                m_maxDstNbSamples = dstNbSamples;
            }

            // 执行重采样
            ret = swr_convert(m_swrCtx.get(), m_dstData, dstNbSamples, const_cast<const uint8_t**>(m_frame->data),
                              m_frame->nb_samples);
            if (ret < 0)
            {
                std::cerr << "[MediaDecoder] Error while converting\n";
                break;
            }

            // 计算输出数据大小
            int dataSize = av_samples_get_buffer_size(&m_dstLinesize, OUTPUT_CHANNELS, ret, OUTPUT_SAMPLE_FMT, 1);

            // 调用回调函数
            if (callback && dataSize > 0)
            {
                callback(m_dstData[0], dataSize);
            }
        }
    }

    av_packet_unref(m_packet.get());
    return true;
}

/**
 * @brief 关闭解码器
 *
 * 释放所有 FFmpeg 相关资源
 */
void MediaDecoder::closeDevice() noexcept
{
    freeDstBuffer();
    m_swrCtx.reset();
    m_decCtx.reset();
    m_fmtCtx.reset();
    m_maxDstNbSamples = 0;
    m_audioStreamIndex = -1;
    m_currentTime = 0.0;
}

/**
 * @brief 获取当前播放时间
 * @return 当前时间（秒）
 */
double MediaDecoder::getCurrentTime() const noexcept
{
    return m_currentTime;
}

/**
 * @brief 获取媒体总时长
 * @return 总时长（秒），无法获取时返回 0
 */
double MediaDecoder::getDuration() const noexcept
{
    if (m_fmtCtx && m_fmtCtx->duration != AV_NOPTS_VALUE)
    {
        return static_cast<double>(m_fmtCtx->duration) / AV_TIME_BASE;
    }
    return 0.0;
}

/**
 * @brief 跳转到指定时间点
 * @param seconds 目标时间（秒）
 * @return true 跳转成功
 * @return false 跳转失败
 *
 * 使用关键帧跳转，跳转后刷新解码器缓冲区
 */
bool MediaDecoder::seek(double seconds) noexcept
{
    if (!m_fmtCtx || m_audioStreamIndex < 0)
    {
        return false;
    }

    AVStream* stream = m_fmtCtx->streams[m_audioStreamIndex];
    int64_t targetTs = av_rescale_q(static_cast<int64_t>(seconds * AV_TIME_BASE), AV_TIME_BASE_Q, stream->time_base);

    if (av_seek_frame(m_fmtCtx.get(), m_audioStreamIndex, targetTs, AVSEEK_FLAG_BACKWARD) < 0)
    {
        std::cerr << "[MediaDecoder] Error seeking to " << seconds << '\n';
        return false;
    }

    if (m_decCtx)
    {
        avcodec_flush_buffers(m_decCtx.get());
    }

    m_currentTime = seconds;
    return true;
}

/**
 * @brief 获取媒体信息
 * @return 媒体信息结构
 */
MediaDecoder::MediaInfo MediaDecoder::getMediaInfo() const noexcept
{
    MediaInfo info{};
    info.duration = getDuration();
    info.currentTime = m_currentTime;

    if (m_decCtx)
    {
        info.sampleRate = m_decCtx->sample_rate;
        info.channels = m_decCtx->channels;
    }
    else
    {
        info.sampleRate = 0;
        info.channels = 0;
    }

    return info;
}

/**
 * @brief 释放重采样输出缓冲区
 */
void MediaDecoder::freeDstBuffer() noexcept
{
    if (m_dstData)
    {
        av_freep(&m_dstData[0]);
        av_freep(&m_dstData);
        m_dstData = nullptr;
    }
    m_dstLinesize = 0;
}
