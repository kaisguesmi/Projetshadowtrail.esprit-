# Makefile for Simple SDL 1.2 Enemy Demo

# Compiler
CC = gcc

# Executable name
TARGET = mygame

# Source files
SRCS = main.c ennemi.c

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Compiler flags:
# -Wall -Wextra: Enable most warnings
# -g: Add debug information
# $(shell sdl-config --cflags): Get SDL include paths from sdl-config
CFLAGS = -Wall -Wextra -g $(shell sdl-config --cflags)

# Linker flags:
# $(shell sdl-config --libs): Get SDL library linking info (path and -lSDL)
# -lSDL_image: Link against the SDL_image library
# -lm: Link against the standard math library (libm) <--- ADDED HERE
LIBS = $(shell sdl-config --libs) -lSDL_image -lm
# Optional: If SDL_gfx is used for flipping, add: -lSDL_gfx

# Default target: build the executable
all: $(TARGET)

# Rule to link the executable from object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "$(TARGET) built successfully."

# Rule to compile a .c file into a .o file
# Depends on the .c file and the shared header file
%.o: %.c ennemi.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleaned project files."

# Rule to run the game (builds first if needed)
run: all
	./$(TARGET)

# Phony targets are rules that don't represent actual files
.PHONY: all clean run
