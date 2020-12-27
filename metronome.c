// SPDX-FileCopyrightText: 2019 Tuomas Siipola
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
        const size_t duration = *size / 50;
        for (size_t i = 0; i < duration; i++) {
            double t = (double)i / duration;

            // Generate sample.
            double sample = 0.0;
            switch (sound) {
                case SOUND_SINE:
                    sample = sin((2 * M_PI * i * freq) / sample_rate);
                    break;
                case SOUND_SQUARE:
                    sample = sgn(sin((2 * M_PI * i * freq) / sample_rate));
                    break;
                case SOUND_SAW:
                    sample = 2 * fmod(i * freq / sample_rate - 0.5, 1) - 1;
                    break;
                case SOUND_TRIANGLE:
                    sample = 4 * fabs(fmod(i * freq / sample_rate - 0.25, 1) - 0.5) - 1;
                    break;
            }

            // Fade sound in and out to reduce audio pops and clicks.
            if (t < 0.1) {
                sample *= t / 0.1;
            } else if (t > 0.75) {
                sample *= 1.0 - ((t - 0.75) / (1.0 - 0.75));
            }

            // Write to output.
            (*buffer)[j * beat + i] = INT16_MAX * sample;
        }
    }
}
