/**
 * @file MediaDecoder.h
 * @brief 媒体解码器管理类
 * @details 负责音视频媒体的解码和处理
 */

#ifndef LVGL_APP_HAL_MEDIA_DECODER_H
#define LVGL_APP_HAL_MEDIA_DECODER_H

#include <functional>
#include <iostream>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

class MediaDecoder
{
  public:
    MediaDecoder();
    ~MediaDecoder();

    bool open(const char *url);
    bool decode(std::function<void(uint8_t *, int)> callback);
    void close();

    double getDuration() const;
    double getCurrentTime() const;
    bool seek(double timestamp);

  private:
    AVFormatContext *m_fmtCtx;
    AVCodecContext *m_decCtx;
    int m_audioStreamIndex;
    AVPacket *m_packet;
    AVFrame *m_frame;
    SwrContext *m_swrCtx;

    uint8_t **m_dstData;
    int m_dstLinesize;
    int m_maxDstNbSamples;
    double m_currentTime;
};

#endif // LVGL_APP_HAL_MEDIA_DECODER_H
