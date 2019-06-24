project('tahdin', 'c')
alsa_dep = dependency('alsa')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
src = ['alsa.c', 'main.c', 'wav.c']
executable('tahdin', src, dependencies : [alsa_dep, m_dep])
