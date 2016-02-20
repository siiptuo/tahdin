#include "alsa.h"

#include <alsa/asoundlib.h>

static snd_pcm_t *handle = NULL;

void alsa_init(unsigned int sample_rate)
{
    int err;

    if ((err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "ALSA ERROR: Playback open error (%s)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16, SND_PCM_ACCESS_RW_INTERLEAVED, 1, sample_rate, 1, 500000)) < 0) {
        fprintf(stderr, "ALSA ERROR: Playback open error (%s)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    if ((err = snd_pcm_prepare(handle)) < 0) {
        fprintf(stderr, "ALSA ERROR: Cannot prepare audio interface for use (%s)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void alsa_play(uint16_t *buffer, size_t size)
{
    int err;

    snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer, size);
    if (frames < 0) {
        frames = snd_pcm_recover(handle, frames, 0);
    }
    if (frames < 0) {
        printf("ALSA ERROR: write failed (%s)\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    if (frames > 0 && frames < (long)size) {
        printf("ALSA ERROR: Short write (expected %li, wrote %li)\n", (long)size, frames);
        exit(EXIT_FAILURE);
    }
}

void alsa_free()
{
    snd_pcm_close(handle);
}
