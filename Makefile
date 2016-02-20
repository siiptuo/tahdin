all: main.c alsa.c
	gcc main.c alsa.c `pkg-config --libs --cflags alsa` -lm -o tahdin
