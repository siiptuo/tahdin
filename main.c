// Tahdin - simple metronome
// Copyright (c) 2016 Tuomas Siipola
// See LICENSE file for license details.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "alsa.h"
#include "wav.h"

static void bye(void)
{
    alsa_free();
}

static double sgn(double x)
{
    if (x > 0) {
        return 1;
    }
    if (x < 0) {
        return -1;
    }
    return 0;
}

typedef enum {
    SOUND_SINE,
    SOUND_SQUARE,
    SOUND_SAW,
} Sound;

int main(int argc, char *argv[])
{
    const unsigned int sample_rate = 48000;

    char *output = NULL;
    Sound sound = SOUND_SINE;

    int c;
    while ((c = getopt(argc, argv, "o:s:")) != -1) {
        switch (c) {
            case 'o':
                output = optarg;
                break;
            case 's':
                if (strcmp(optarg, "sine") == 0) {
                    sound = SOUND_SINE;
                } else if (strcmp(optarg, "square") == 0) {
                    sound = SOUND_SQUARE;
                } else if (strcmp(optarg, "saw") == 0) {
                    sound = SOUND_SAW;
                } else {
                    fprintf(stderr, "sound %s is invalid, expected sine, square or saw", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                abort();
        }
    }

    char *end;

    if (argc - optind != 1 && argc - optind != 2) {
        printf("Usage: tahdin [options] [time signature] tempo\n");
        exit(EXIT_SUCCESS);
    }

    long tempo = strtol(argv[argc - 1], &end, 10);
    if (*end != '\0' || tempo <= 0) {
        fprintf(stderr, "invalid tempo value: '%s'\n", argv[argc - 1]);
        exit(EXIT_FAILURE);
    }

    long numerator = 4;
    long denominator = 4;

    if (argc - optind == 2) {
        numerator = strtol(argv[argc - 2], &end, 10);
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

    size_t beat = 4 * 60 * sample_rate / tempo / denominator;
    size_t size = beat * numerator;
    uint16_t buffer[size];
    memset(buffer, 0, size * sizeof(buffer[0]));

    for (size_t j = 0; j < numerator; j++) {
        const double freq = (j == 0) ? 523.25 : 261.63;
        for (size_t i = 0; i < size / 50; i++) {
            switch (sound) {
                case SOUND_SINE:
                    buffer[j * beat + i] = INT16_MAX * sin((2 * M_PI * i * freq) / sample_rate);
                    break;
                case SOUND_SQUARE:
                    buffer[j * beat + i] = INT16_MAX * sgn(sin((2 * M_PI * i * freq) / sample_rate));
                    break;
                case SOUND_SAW:
                    buffer[j * beat + i] = INT16_MAX * (2 * fmod(i * freq / sample_rate, 1) - 1);
                    break;
            }
        }
    }

    if (output) {
        wav_write(output, sample_rate, buffer, size, 4);
    } else {
        alsa_init(sample_rate);
        int err;
        if ((err = atexit(bye)) != 0) {
            fprintf(stderr, "ERROR: cannot set exit function\n");
            exit(EXIT_FAILURE);
        }
        while (true) {
            alsa_play(buffer, size);
        }
    }

    return EXIT_SUCCESS;
}
