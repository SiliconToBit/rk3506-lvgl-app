#include "AudioDevice.h"
#include <iostream>

AudioDevice::AudioDevice()
    : m_pcmHandle(nullptr)
    , m_deviceName("default")
    , m_mixerName("Master")
{
}

AudioDevice::~AudioDevice()
{
    close();
}

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

    if ((err = snd_pcm_set_params(m_pcmHandle,
                                  SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED, channels, rate,
                                  1,
                                  500000)) < 0)
    {
        std::cerr << "Playback open error: " << snd_strerror(err) << std::endl;
        close();
        return false;
    }

    return true;
}

void AudioDevice::close()
{
    if (m_pcmHandle)
    {
        snd_pcm_drain(m_pcmHandle);
        snd_pcm_close(m_pcmHandle);
        m_pcmHandle = nullptr;
    }
}

snd_pcm_sframes_t AudioDevice::write(const void* buffer, snd_pcm_uframes_t frames)
{
    if (!m_pcmHandle)
        return -1;

    snd_pcm_sframes_t frames_written = snd_pcm_writei(m_pcmHandle, buffer, frames);

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

void AudioDevice::prepare()
{
    if (m_pcmHandle)
    {
        snd_pcm_prepare(m_pcmHandle);
    }
}

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
    snd_mixer_selem_id_set_name(sid, m_mixerName.c_str());
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
