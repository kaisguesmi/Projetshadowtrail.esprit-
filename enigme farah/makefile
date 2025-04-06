# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -std=c99 `sdl-config --cflags`
LDFLAGS = `sdl-config --libs` -lSDL_ttf -lSDL_image

# Source files
SRC = main.c puzzle.c
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = puzzle_game

# Default target
all: $(EXEC)

# Build rule
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Compile .c to .o
%.o: %.c puzzle.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ) $(EXEC)

# Install dependencies (Ubuntu)
install-deps:
	sudo apt-get install libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev make

# Run the game
run: $(EXEC)
	./$(EXEC)

.PHONY: all clean install-deps run
