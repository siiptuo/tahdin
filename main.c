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

static void bye(void) {
    alsa_free();
}

int main(int argc, char *argv[])
{
    const unsigned int sample_rate = 48000;

    char *output = NULL;

    int c;
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o':
                output = optarg;
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
        const int freq = (j == 0) ? 523.25 : 261.63;
        for (size_t i = 0; i < size / 50; i++) {
            buffer[j * beat + i] = INT16_MAX * sin((2 * M_PI * i * freq) / sample_rate);
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
