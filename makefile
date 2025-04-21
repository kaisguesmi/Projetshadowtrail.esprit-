# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -pedantic -std=c99
SDL_FLAGS = `sdl-config --cflags --libs` -lSDL_image -lSDL_ttf -lSDL_mixer

# Fichiers sources
SRC = main.c puzzle.c
OBJ = $(SRC:.c=.o)
EXEC = puzzle_game

# Règle par défaut
all: $(EXEC)

# Règle de compilation
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(SDL_FLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(SDL_FLAGS)

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC)

# Installation des dépendances (pour Ubuntu/Debian)
install-deps:
	sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libsdl-mixer1.2-dev

.PHONY: all clean install-deps
