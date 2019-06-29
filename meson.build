# Copyright 2019 Tuomas Siipola
# SPDX-License-Identifier: GPL-3.0-or-later

project('tahdin', 'c')
alsa_dep = dependency('alsa')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
src = ['alsa.c', 'main.c', 'metronome.c', 'wav.c']
executable('tahdin', src, dependencies : [alsa_dep, m_dep])
