// SPDX-FileCopyrightText: 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WAV_H
#define WAV_H

#include <stdlib.h>
#include <stdint.h>

extern void wav_write(const char *path, size_t sample_rate, uint16_t *buffer, size_t size, size_t repeats);

#endif // WAV_H
