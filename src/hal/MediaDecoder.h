//
// Created by gm on 2025/12/5.
//

#ifndef LVGL_DEMO_MEDIADECODER_H
#define LVGL_DEMO_MEDIADECODER_H

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
    // 解码一帧或一段数据，通过回调返回 PCM 数据
    bool decode(std::function<void(uint8_t *, int)> callback);
    void close();

    double getDuration() const;
    double getCurrentTime() const;
    bool seek(double timestamp);

  private:
    AVFormatContext *fmt_ctx = nullptr;
    AVCodecContext *dec_ctx = nullptr;
    int audio_stream_index = -1;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    SwrContext *swr_ctx = nullptr;

    // 重采样输出缓冲区
    uint8_t **dst_data = nullptr;
    int dst_linesize;
    int max_dst_nb_samples = 0;

    //当前播放时间（秒）
    double current_time = 0.0;
};

#endif // LVGL_DEMO_MEDIADECODER_H