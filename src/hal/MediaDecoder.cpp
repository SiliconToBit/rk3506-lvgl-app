//
// Created by gm on 2025/12/5.
//

#include "MediaDecoder.h"

MediaDecoder::MediaDecoder()
{
    packet = av_packet_alloc();
    frame = av_frame_alloc();
}

MediaDecoder::~MediaDecoder()
{
    close();
    if (packet)
        av_packet_free(&packet);
    if (frame)
        av_frame_free(&frame);
}

/**
 * @brief 打开并配置一个音频解码设备。
 *
 * @param url     输入媒体文件。
 *
 * @return true   打开并配置成功，PCM 设备可以正常用于播放。
 * @return false  打开或配置失败，会在标准错误输出打印错误信息。
 */
bool MediaDecoder::open(const char *url)
{
    int ret;

    // AVFormatContext 对象创建
    fmt_ctx = avformat_alloc_context();
    // 1. 打开输入文件
    if ((ret = avformat_open_input(&fmt_ctx, url, nullptr, nullptr)) < 0)
    {
        std::cerr << "Could not open source file " << url << std::endl;
        return false;
    }

    // 输出音频文件的信息
    // av_dump_format(fmt_ctx, 0, url, 0);

    // 2. 获取流信息
    if ((ret = avformat_find_stream_info(fmt_ctx, nullptr)) < 0)
    {
        std::cerr << "Could not find stream information" << std::endl;
        return false;
    }

    // 3. 查找音频流，获取解码器
    audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audio_stream_index < 0)
    {
        std::cerr << "Could not find audio stream in input file" << std::endl;
        return false;
    }

    AVStream *audio_stream = fmt_ctx->streams[audio_stream_index];
    const AVCodec *decoder = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    if (!decoder)
    {
        std::cerr << "Failed to find decoder codec" << std::endl;
        return false;
    }

    // 4. 分配解码器上下文
    dec_ctx = avcodec_alloc_context3(decoder);
    if (!dec_ctx)
    {
        std::cerr << "Failed to allocate codec context" << std::endl;
        return false;
    }

    // 5. 将流参数复制到解码器上下文
    if (avcodec_parameters_to_context(dec_ctx, audio_stream->codecpar) < 0)
    {
        std::cerr << "Failed to copy codec parameters to decoder context" << std::endl;
        return false;
    }

    // 6. 打开解码器
    if (avcodec_open2(dec_ctx, decoder, nullptr) < 0)
    {
        std::cerr << "Failed to open codec" << std::endl;
        return false;
    }

    // 7. 初始化重采样上下文
    swr_ctx = swr_alloc();
    if (!swr_ctx)
    {
        std::cerr << "Could not allocate resampler context" << std::endl;
        return false;
    }

    // 设置重采样选项
    // 输入参数
    av_opt_set_int(swr_ctx, "in_channel_layout",
                   dec_ctx->channel_layout ? dec_ctx->channel_layout : av_get_default_channel_layout(dec_ctx->channels),
                   0);
    av_opt_set_int(swr_ctx, "in_sample_rate", dec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", dec_ctx->sample_fmt, 0);

    // 输出参数 (固定为 S16LE, Stereo, 44100Hz)
    av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

    if ((ret = swr_init(swr_ctx)) < 0)
    {
        std::cerr << "Failed to initialize the resampling context" << std::endl;
        return false;
    }

    return true;
}

bool MediaDecoder::decode(std::function<void(uint8_t *, int)> callback)
{
    if (!fmt_ctx || !dec_ctx)
        return false;

    int ret = av_read_frame(fmt_ctx, packet);
    if (ret < 0)
    {
        // End of file or error
        return false;
    }

    if (packet->stream_index == audio_stream_index)
    {
        ret = avcodec_send_packet(dec_ctx, packet);
        if (ret < 0)
        {
            std::cerr << "Error submitting the packet to the decoder" << std::endl;
            av_packet_unref(packet);
            return false;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_frame(dec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0)
            {
                std::cerr << "Error during decoding" << std::endl;
                av_packet_unref(packet);
                return false;
            }

            if (frame->pts != AV_NOPTS_VALUE)
            {
                // std::cout << "Decoded frame PTS: " << frame->pts << std::endl;
                AVStream *stream = fmt_ctx->streams[audio_stream_index];
                current_time = frame->pts * av_q2d(stream->time_base);
            }

            // 计算输出样本数
            int dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, dec_ctx->sample_rate) + frame->nb_samples, 44100,
                                                dec_ctx->sample_rate, AV_ROUND_UP);

            if (dst_nb_samples > max_dst_nb_samples)
            {
                if (dst_data)
                {
                    av_freep(&dst_data[0]);
                    av_freep(&dst_data);
                }
                av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, 2, dst_nb_samples, AV_SAMPLE_FMT_S16, 0);
                max_dst_nb_samples = dst_nb_samples;
            }

            ret = swr_convert(swr_ctx, dst_data, dst_nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
            if (ret < 0)
            {
                std::cerr << "Error while converting" << std::endl;
                break;
            }

            int data_size = av_samples_get_buffer_size(&dst_linesize, 2, ret, AV_SAMPLE_FMT_S16, 1);

            if (callback && data_size > 0)
            {
                callback(dst_data[0], data_size);
            }
        }
    }

    av_packet_unref(packet);
    return true;
}

void MediaDecoder::close()
{
    if (dst_data)
    {
        av_freep(&dst_data[0]);
        av_freep(&dst_data);
        dst_data = nullptr;
    }
    if (swr_ctx)
    {
        swr_free(&swr_ctx);
        swr_ctx = nullptr;
    }
    if (dec_ctx)
    {
        avcodec_free_context(&dec_ctx);
        dec_ctx = nullptr;
    }
    if (fmt_ctx)
    {
        avformat_close_input(&fmt_ctx);
        fmt_ctx = nullptr;
    }
    max_dst_nb_samples = 0;
}

double MediaDecoder::getCurrentTime() const
{
    return current_time;
}

double MediaDecoder::getDuration() const
{
    if (fmt_ctx && fmt_ctx->duration != AV_NOPTS_VALUE)
    {
        // duration 单位是 AV_TIME_BASE (微秒)，转换为秒
        return (double)fmt_ctx->duration / AV_TIME_BASE;
    }
    return 0.0;
}


bool MediaDecoder::seek(double seconds)
{
    if (!fmt_ctx || audio_stream_index < 0)
        return false;

    AVStream *stream = fmt_ctx->streams[audio_stream_index];
    // 将秒转换为流的时间基单位
    int64_t target_ts = av_rescale_q((int64_t)(seconds * AV_TIME_BASE), AV_TIME_BASE_Q, stream->time_base);

    // 跳转到关键帧
    // AVSEEK_FLAG_BACKWARD: 向后查找最近的关键帧
    if (av_seek_frame(fmt_ctx, audio_stream_index, target_ts, AVSEEK_FLAG_BACKWARD) < 0)
    {
        std::cerr << "Error seeking to " << seconds << std::endl;
        return false;
    }

    // 清空解码器缓存，防止解码错误
    if (dec_ctx)
    {
        avcodec_flush_buffers(dec_ctx);
    }

    // 更新当前时间记录
    current_time = seconds;

    return true;
}

