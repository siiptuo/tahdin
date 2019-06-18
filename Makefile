all: main.c alsa.c wav.c
	gcc main.c alsa.c wav.c `pkg-config --libs --cflags alsa` -lm -o tahdin
