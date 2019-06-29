// Copyright 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <math.h>

#include "metronome.h"

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

void metronome_generate(uint16_t **buffer, size_t *size, unsigned int sample_rate, long tempo, int numerator, int denominator, Sound sound)
{
    size_t beat = 4 * 60 * sample_rate / tempo / denominator;
    *size = beat * numerator;
    *buffer = calloc(*size, sizeof(uint16_t));
    if (!*buffer) {
        return;
    }

    for (size_t j = 0; j < numerator; j++) {
        const double freq = (j == 0) ? 523.25 : 261.63;
        for (size_t i = 0; i < *size / 50; i++) {
            switch (sound) {
                case SOUND_SINE:
                    (*buffer)[j * beat + i] = INT16_MAX * sin((2 * M_PI * i * freq) / sample_rate);
                    break;
                case SOUND_SQUARE:
                    (*buffer)[j * beat + i] = INT16_MAX * sgn(sin((2 * M_PI * i * freq) / sample_rate));
                    break;
                case SOUND_SAW:
                    (*buffer)[j * beat + i] = INT16_MAX * (2 * fmod(i * freq / sample_rate, 1) - 1);
                    break;
                case SOUND_TRIANGLE:
                    (*buffer)[j * beat + i] = INT16_MAX * (2 * fabs(fmod(i * freq / sample_rate, 2) - 1) - 1);
                    break;
            }
        }
    }
}
