CC = gcc
CFLAGS = -Wall -g `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lm
TARGET = enemydemo
SRCS = main.c ennemi.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# Dépendances
main.o: main.c ennemi.h
ennemi.o: ennemi.c ennemi.h

.PHONY: all clean
