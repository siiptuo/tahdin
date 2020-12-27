// SPDX-FileCopyrightText: 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "metronome.h"
#include "alsa.h"
#include "wav.h"

static void bye(void)
{
    alsa_free();
}

static void usage()
{
    printf("Usage: tahdin [options] [time signature] tempo\n");
    printf("Options:\n");
    printf("  -o, --output=FILE  export WAV file\n");
    printf("  -s, --sound=SOUND  play sine, square, saw or triangle\n");
    printf("  -h, --help         display this help and exit\n");
    printf("  -V, --version      display version information and exit\n");
}

int main(int argc, char *argv[])
{
    const unsigned int sample_rate = 48000;

    char *output = NULL;
    Sound sound = SOUND_SINE;

    static struct option long_options[] = {
        { "output",  required_argument, NULL, 'o' },
        { "sound",   required_argument, NULL, 's' },
        { "help",    no_argument,       NULL, 'h' },
        { "version", no_argument,       NULL, 'V' },
        { NULL,      0,                 NULL, 0 },
    };

    int c;
    while ((c = getopt_long(argc, argv, "o:s:hV", long_options, NULL)) != -1) {
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
                } else if (strcmp(optarg, "triangle") == 0) {
                    sound = SOUND_TRIANGLE;
                } else {
                    fprintf(stderr, "sound %s is invalid, expected sine, square, saw or triangle", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'V':
                printf("tahdin 0.0\n");
                return EXIT_SUCCESS;
            default:
                abort();
        }
    }

    char *end;

    if (argc - optind != 1 && argc - optind != 2) {
        usage();
        return EXIT_SUCCESS;
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

    uint16_t *buffer = NULL;
    size_t size;
    metronome_generate(&buffer, &size, sample_rate, tempo, numerator, denominator, sound);

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
