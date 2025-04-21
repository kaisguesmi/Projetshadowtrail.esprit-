# Compiler and flags
CC = gcc
# CFLAGS: -Wall enables warnings, -g adds debug symbols, -O2 optimizes (optional)
CFLAGS = -Wall -Wextra -g -O0 # Use -O0 for easier debugging, -O2 for optimization
# LDFLAGS: Libraries to link against (-lSDL, -lSDL_image, -lSDL_ttf, -lm for math)
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lm

# Executable name
EXEC = minimap_test

# Source files
SRCS = main.c minimap.c

# Object files (automatically generated from SRCS)
OBJS = $(SRCS:.c=.o)

# Header files (dependency for object files)
HDRS = minimap.h

# Default target: build the executable
all: $(EXEC)

# Rule to link the executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	@echo "$(EXEC) built successfully."

# Rule to compile source files into object files
# $< is the first prerequisite (.c file)
# $@ is the target (.o file)
# $(HDRS) makes sure objects recompile if the header changes
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Target to clean up build files
clean:
	rm -f $(OBJS) $(EXEC)
	@echo "Cleaned build files."

# Phony targets are not actual files
.PHONY: all clean
