# Compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -Wextra -g -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT

# Bibliothèques à lier
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

# Fichiers source
SRCS = main.c interface1.c interface2.c enigme.c

# Fichiers objets
OBJS = $(SRCS:.c=.o)

# Nom de l'exécutable
TARGET = integrated_app

# Règle par défaut
all: $(TARGET)

# Règle pour créer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Règle pour compiler les fichiers source en fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règles de nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all


