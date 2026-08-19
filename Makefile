CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard src/*.c)
OBJFILES = $(patsubst src/%.c, obj/%.o, $(SRC))
TARGET = my_audio_player

# detect OS
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lpthread -lm -L./pdcursesmod/wincon -lpdcurses
    CURSES_INCLUDE =

		# set commands on windows
		MKDIR = if not exist obj mkdir obj
		RM = del /Q /F
		CLEAN_TARGET = obj\*.o $(TARGET).exe
else
    LDFLAGS = -lpthread -lm -ldl -lncursesw
    CURSES_INCLUDE =

		# set commands on unix
		MKDIR = mkdir -p obj
		RM = rm -rf
		CLEAN_TARGET = $(OBJFILES) $(TARGET) *~
endif

all: pdcurses $(TARGET)

pdcurses:
ifeq ($(OS),Windows_NT)
	@if not exist pdcursesmod git clone https://github.com/Bill-Gray/PDCursesMod.git pdcursesmod
	@if not exist pdcursesmod\wincon\pdcurses.a $(MAKE) -C pdcursesmod\wincon -f Mingw32.mak
endif

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

obj/%.o: src/%.c
	@$(MKDIR)
	$(CC) $(CFLAGS) $(CURSES_INCLUDE) -c $< -o $@

clean:
	$(RM) $(CLEAN_TARGET)

.PHONY: all clean pdcurses
