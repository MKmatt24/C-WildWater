CC = gcc 
CFLAGS = -Wall -Wextra
LIBS = -lm 
SRCDIR = src
INCDIR = include

SRC = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRC:$(SRCDIR)/%.c=%.o)
TARGET = C-WildWater

.SILENT:

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)
	rm -rf $(OBJS)

%.o: $(SRCDIR)/%.c
    $(CC) $(CFLAGS) -c $< -o $@ -I$(INCDIR)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
