# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

# Nom de l'exécutable
TARGET = C-WildWater

# Fichiers sources et objets
SRCS = main.c avl.c functions.c
OBJS = $(SRCS:.c=.o)
HEADERS = avl.h functions.h

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation des fichiers objets
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers générés
clean:
	rm -f $(OBJS) $(TARGET)

# Nettoyage complet (fichiers objets + données générées)
fclean: clean
	rm -f *.dat *.csv *.png

# Recompilation complète
re: fclean all

.PHONY: all clean fclean re
