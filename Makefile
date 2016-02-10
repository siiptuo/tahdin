all: main.c
	gcc main.c `pkg-config --libs --cflags alsa` -lm -o tahdin
