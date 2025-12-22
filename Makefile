CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lncurses

TARGET = editor
SRC = ncurses_test.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

