CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard src/*.c)
OBJFILES = $(patsubst src/%.c, obj/%.o, $(SRC))
TARGET = my_audio_player

# detect OS
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lpthread -lm -L./pdcursesmod -lpdcurses
    CURSES_INCLUDE = -I./pdcursesmod

		# set commands on windows
		MKDIR = if not exist obj mkdir obj
else
    LDFLAGS = -lpthread -lm -ldl -lncursesw
    CURSES_INCLUDE =

		# set commands on unix
		MKDIR = mkdir -p obj
endif

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

obj/%.o: src/%.c
	@$(MKDIR)
	$(CC) $(CFLAGS) $(CURSES_INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJFILES) $(TARGET) *~

.PHONY: all clean
