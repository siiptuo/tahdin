// Copyright 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ALSA_H
#define ALSA_H

#include <stdlib.h>
#include <stdint.h>

extern void alsa_init(unsigned int sample_rate);
extern void alsa_play(uint16_t *buffer, size_t size);
extern void alsa_free();

#endif // ALSA_H
