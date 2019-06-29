# Copyright 2019 Tuomas Siipola
# SPDX-License-Identifier: GPL-3.0-or-later

project('tahdin', 'c')

alsa_dep = dependency('alsa')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
gtk_dep = dependency('gtk+-3.0')

cli_src = ['alsa.c', 'main.c', 'metronome.c', 'wav.c']
executable('tahdin', cli_src, dependencies : [alsa_dep, m_dep])

gtk_src = ['alsa.c', 'gtk.c', 'metronome.c']
executable('tahdin-gtk', gtk_src, dependencies : [alsa_dep, gtk_dep, m_dep])
