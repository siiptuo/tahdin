// Tahdin - simple metronome
// Copyright (c) 2016 Tuomas Siipola
// See LICENSE file for license details.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include <alsa/asoundlib.h>

snd_pcm_t *handle = NULL;

void bye(void) {
    snd_pcm_close(handle);
}

int main(int argc, char *argv[])
{
    const unsigned int sample_rate = 48000;

    if (argc < 2 || argc > 3) {
        printf("Usage: tahdin [time signature] tempo\n");
        exit(EXIT_SUCCESS);
    }

    char *end;

    long tempo = strtol(argv[argc - 1], &end, 10);
    if (*end != '\0' || tempo <= 0) {
        fprintf(stderr, "invalid tempo value: '%s'\n", argv[argc - 1]);
        exit(EXIT_FAILURE);
    }

    long numerator = 4;
    long denominator = 4;

    if (argc == 3) {
        numerator = strtol(argv[1], &end, 10);
        if (*end != '/' || numerator <= 0) {
            fprintf(stderr, "invalid time signature: '%s'\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        denominator = strtol(end + 1, &end, 10);
        if (*end != '\0' || denominator <= 0) {
            fprintf(stderr, "invalid time signature: '%s'\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    int err;

    if ((err = atexit(bye)) != 0) {
        fprintf(stderr, "ERROR: cannot set exit function\n");
        exit(EXIT_FAILURE);
    }

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

    size_t beat = 4 * 60 * sample_rate / tempo / denominator;
    size_t size = beat * numerator;
    uint16_t buffer[size];
    memset(buffer, 0, size * sizeof(buffer[0]));

    for (size_t j = 0; j < numerator; j++) {
        const int freq = (j == 0) ? 523.25 : 261.63;
        for (size_t i = 0; i < size / 50; i++) {
            buffer[j * beat + i] = INT16_MAX * sin((2 * M_PI * i * freq) / sample_rate);
        }
    }

    while (true) {
        snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer, size);
        if (frames < 0) {
            frames = snd_pcm_recover(handle, frames, 0);
        }
        if (frames < 0) {
            printf("ALSA ERROR: write failed (%s)\n", snd_strerror(err));
            break;
        }
        if (frames > 0 && frames < (long)size) {
            printf("ALSA ERROR: Short write (expected %li, wrote %li)\n", (long)size, frames);
        }
    }

    return EXIT_SUCCESS;
}
