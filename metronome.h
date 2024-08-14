// SPDX-FileCopyrightText: 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef METRONOME_H
#define METRONOME_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    SOUND_SINE,
    SOUND_SQUARE,
    SOUND_SAW,
    SOUND_TRIANGLE,
} Sound;

extern void metronome_generate(uint16_t **buffer, size_t *size, unsigned int sample_rate, long tempo, int numerator, int denominator, Sound sound, float volume);

#endif // METRONOME_H
