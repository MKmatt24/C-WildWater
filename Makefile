CC = gcc
# Ajout de -Iinclude pour que le compilateur trouve les headers
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude
LDFLAGS = -lm

TARGET = C-WildWater

# Indiquer que les sources sont dans src/
SRCS = src/main.c src/avl.c src/functions.c
# Les objets seront créés au même endroit que les sources
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Règle pour compiler les .o à partir des .c dans src/
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

fclean: clean
	rm -f *.dat *.csv *.png

re: fclean all

.PHONY: all clean fclean re
