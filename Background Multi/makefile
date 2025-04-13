# Makefile pour le jeu SDL 1.2

# Compilateur et options
CC      := gcc
CFLAGS  := -Wall -g `sdl-config --cflags` -I.
LDFLAGS := `sdl-config --libs` -lSDL_image -lSDL_ttf

# Sources, objets et binaire final
SRCS    := main.c bg.c
OBJS    := $(SRCS:.c=.o)
TARGET  := game

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c bg.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

