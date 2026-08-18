CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard src/*.c)
OBJFILES = $(patsubst src/%.c, obj/%.o, $(SRC))
TARGET = my_audio_player

# detect OS
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lpthread -lm -L./pdcursesmod -lpdcurses
    CURSES_INCLUDE = -I./pdcursesmod
else
    LDFLAGS = -lpthread -lm -ldl -lncursesw
    CURSES_INCLUDE =
endif

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(CURSES_INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJFILES) $(TARGET) *~

.PHONY: all clean
