#include "MediaDecoder.h"

/**
 * @brief 构造函数
 * @details 初始化FFmpeg相关结构体指针并分配packet和frame
 */
MediaDecoder::MediaDecoder()
    : m_fmtCtx(nullptr), m_decCtx(nullptr), m_audioStreamIndex(-1), m_packet(nullptr), m_frame(nullptr),
      m_swrCtx(nullptr), m_dstData(nullptr), m_dstLinesize(0), m_maxDstNbSamples(0), m_currentTime(0.0)
{
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
}

/**
 * @brief 析构函数
 * @details 释放所有FFmpeg资源
 */
MediaDecoder::~MediaDecoder()
{
    close();
    if (m_packet)
        av_packet_free(&m_packet);
    if (m_frame)
        av_frame_free(&m_frame);
}

/**
 * @brief 打开媒体文件
 * @param url 文件路径
 * @return true 打开成功
 * @return false 打开失败
 * @details 打开文件,查找音频流,初始化解码器和重采样器
 */
bool MediaDecoder::open(const char *url)
{
    int ret;

    m_fmtCtx = avformat_alloc_context();
    if ((ret = avformat_open_input(&m_fmtCtx, url, nullptr, nullptr)) < 0)
    {
        std::cerr << "Could not open source file " << url << std::endl;
        return false;
    }

    if ((ret = avformat_find_stream_info(m_fmtCtx, nullptr)) < 0)
    {
        std::cerr << "Could not find stream information" << std::endl;
        return false;
    }

    m_audioStreamIndex = av_find_best_stream(m_fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (m_audioStreamIndex < 0)
    {
        std::cerr << "Could not find audio stream in input file" << std::endl;
        return false;
    }

    AVStream *audio_stream = m_fmtCtx->streams[m_audioStreamIndex];
    const AVCodec *decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    if (!decoder)
    {
        std::cerr << "Failed to find decoder codec" << std::endl;
        return false;
    }

    m_decCtx = avcodec_alloc_context3(decoder);
    if (!m_decCtx)
    {
        std::cerr << "Failed to allocate codec context" << std::endl;
        return false;
    }

    if (avcodec_parameters_to_context(m_decCtx, audio_stream->codecpar) < 0)
    {
        std::cerr << "Failed to copy codec parameters to decoder context" << std::endl;
        return false;
    }

    if (avcodec_open2(m_decCtx, decoder, nullptr) < 0)
    {
        std::cerr << "Failed to open codec" << std::endl;
        return false;
    }

    m_swrCtx = swr_alloc();
    if (!m_swrCtx)
    {
        std::cerr << "Could not allocate resampler context" << std::endl;
        return false;
    }

    av_opt_set_int(
        m_swrCtx, "in_channel_layout",
        m_decCtx->channel_layout ? m_decCtx->channel_layout : av_get_default_channel_layout(m_decCtx->channels), 0);
    av_opt_set_int(m_swrCtx, "in_sample_rate", m_decCtx->sample_rate, 0);
    av_opt_set_sample_fmt(m_swrCtx, "in_sample_fmt", m_decCtx->sample_fmt, 0);

    av_opt_set_int(m_swrCtx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(m_swrCtx, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(m_swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    if ((ret = swr_init(m_swrCtx)) < 0)
    {
        std::cerr << "Failed to initialize the resampling context" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief 解码音频数据
 * @param callback 音频数据回调函数
 * @return true 解码成功(可能还有更多数据)
 * @return false 解码完成或失败
 * @details 读取一帧数据,解码并通过重采样转换为16位立体声44100Hz格式
 */
bool MediaDecoder::decode(std::function<void(uint8_t *, int)> callback)
{
    if (!m_fmtCtx || !m_decCtx)
        return false;

    int ret = av_read_frame(m_fmtCtx, m_packet);
    if (ret < 0)
    {
        return false;
    }

    if (m_packet->stream_index == m_audioStreamIndex)
    {
        ret = avcodec_send_packet(m_decCtx, m_packet);
        if (ret < 0)
        {
            std::cerr << "Error submitting the packet to the decoder" << std::endl;
            av_packet_unref(m_packet);
            return false;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_frame(m_decCtx, m_frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0)
            {
                std::cerr << "Error during decoding" << std::endl;
                av_packet_unref(m_packet);
                return false;
            }

            if (m_frame->pts != AV_NOPTS_VALUE)
            {
                AVStream *stream = m_fmtCtx->streams[m_audioStreamIndex];
                m_currentTime = m_frame->pts * av_q2d(stream->time_base);
            }

            int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_swrCtx, m_decCtx->sample_rate) + m_frame->nb_samples,
                                                44100, m_decCtx->sample_rate, AV_ROUND_UP);

            if (dst_nb_samples > m_maxDstNbSamples)
            {
                if (m_dstData)
                {
                    av_freep(&m_dstData[0]);
                    av_freep(&m_dstData);
                }
                av_samples_alloc_array_and_samples(&m_dstData, &m_dstLinesize, 2, dst_nb_samples, AV_SAMPLE_FMT_S16, 0);
                m_maxDstNbSamples = dst_nb_samples;
            }

            ret =
                swr_convert(m_swrCtx, m_dstData, dst_nb_samples, (const uint8_t **)m_frame->data, m_frame->nb_samples);
            if (ret < 0)
            {
                std::cerr << "Error while converting" << std::endl;
                break;
            }

            int data_size = av_samples_get_buffer_size(&m_dstLinesize, 2, ret, AV_SAMPLE_FMT_S16, 1);

            if (callback && data_size > 0)
            {
                callback(m_dstData[0], data_size);
            }
        }
    }

    av_packet_unref(m_packet);
    return true;
}

/**
 * @brief 关闭解码器
 * @details 释放所有FFmpeg相关资源
 */
void MediaDecoder::close()
{
    if (m_dstData)
    {
        av_freep(&m_dstData[0]);
        av_freep(&m_dstData);
        m_dstData = nullptr;
    }
    if (m_swrCtx)
    {
        swr_free(&m_swrCtx);
        m_swrCtx = nullptr;
    }
    if (m_decCtx)
    {
        avcodec_free_context(&m_decCtx);
        m_decCtx = nullptr;
    }
    if (m_fmtCtx)
    {
        avformat_close_input(&m_fmtCtx);
        m_fmtCtx = nullptr;
    }
    m_maxDstNbSamples = 0;
}

/**
 * @brief 获取当前播放时间
 * @return double 当前时间(秒)
 */
double MediaDecoder::getCurrentTime() const
{
    return m_currentTime;
}

/**
 * @brief 获取媒体总时长
 * @return double 总时长(秒)
 */
double MediaDecoder::getDuration() const
{
    if (m_fmtCtx && m_fmtCtx->duration != AV_NOPTS_VALUE)
    {
        return (double)m_fmtCtx->duration / AV_TIME_BASE;
    }
    return 0.0;
}

/**
 * @brief 跳转到指定时间点
 * @param seconds 目标时间(秒)
 * @return true 跳转成功
 * @return false 跳转失败
 * @details 使用关键帧跳转,跳转后刷新解码器缓冲区
 */
bool MediaDecoder::seek(double seconds)
{
    if (!m_fmtCtx || m_audioStreamIndex < 0)
        return false;

    AVStream *stream = m_fmtCtx->streams[m_audioStreamIndex];
    int64_t target_ts = av_rescale_q((int64_t)(seconds * AV_TIME_BASE), AV_TIME_BASE_Q, stream->time_base);

    if (av_seek_frame(m_fmtCtx, m_audioStreamIndex, target_ts, AVSEEK_FLAG_BACKWARD) < 0)
    {
        std::cerr << "Error seeking to " << seconds << std::endl;
        return false;
    }

    if (m_decCtx)
    {
        avcodec_flush_buffers(m_decCtx);
    }

    m_currentTime = seconds;

    return true;
}
