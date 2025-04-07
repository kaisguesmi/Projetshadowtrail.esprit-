
# Compilateur et options
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf

# Fichiers source et objets
SRC = main.c source.c
OBJ = $(SRC:.c=.o)
EXEC = menu

# Règle par défaut
all: $(EXEC)

# Règle pour créer l'exécutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Règle pour compiler les fichiers source en fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(OBJ) $(EXEC)

# Règle pour réinstaller (nettoyer puis recompiler)
re: clean all
