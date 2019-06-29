// Copyright 2019 Tuomas Siipola
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wav.h"

#include <stdio.h>
#include <stdbool.h>

static bool write_uint16(uint16_t i, FILE *fp)
{
    uint8_t buf[] = { i, i >> 8 };
    return fwrite(buf, sizeof buf, 1, fp) == sizeof buf;
}

static bool write_uint32(uint32_t i, FILE *fp)
{
    uint8_t buf[] = { i, i >> 8, i >> 16, i >> 24 };
    return fwrite(buf, sizeof buf, 1, fp) == sizeof buf;
}

void wav_write(const char *path, size_t sample_rate, uint16_t *buffer, size_t size, size_t repeats)
{
    FILE *fp = fopen(path, "w");
    if (!fp) {
        fprintf(stderr, "WAV ERROR: Failed to open file '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    uint32_t fmt_size = 16;
    uint16_t audio_format = 1; // PCM
    uint16_t channels = 1;
    uint16_t bytes_per_sample = sizeof(buffer[0]);
    uint16_t bits_per_sample = bytes_per_sample * 8;
    uint32_t byte_rate = sample_rate * channels * bytes_per_sample;
    uint16_t block_align = channels * bytes_per_sample;

    uint32_t data_size = size * channels * bytes_per_sample * repeats;
    if (data_size % 2 == 1) {
        data_size++;
    }

    uint32_t riff_size = 4 + 8 + fmt_size + 8 + data_size;

    fputs("RIFF", fp);
    write_uint32(riff_size, fp);
    fputs("WAVE", fp);

    fputs("fmt ", fp);
    write_uint32(fmt_size, fp);
    write_uint16(audio_format, fp);
    write_uint16(channels, fp);
    write_uint32(sample_rate, fp);
    write_uint32(byte_rate, fp);
    write_uint16(block_align, fp);
    write_uint16(bits_per_sample, fp);

    fputs("data", fp);
    write_uint32(data_size, fp);
    for (size_t i = 0; i < repeats; i++) {
        for (size_t j = 0; j < size; j++) {
            write_uint16(buffer[j], fp);
        }
    }

    fclose(fp);
}
