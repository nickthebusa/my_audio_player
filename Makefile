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
		RM = del /Q /F
		RMDIR = rmdir /S /Q
		TARGET_BIN = $(TARGET).exe
		CLEAN_OBJS = $(subst /,\\,$(OBJFILES))
else
    LDFLAGS = -lpthread -lm -ldl -lncursesw
    CURSES_INCLUDE =

		# set commands on unix
		MKDIR = mkdir -p obj
		RM = rm -f
		RMDIR = rm -rf obj
		TARGET_BIN = $(TARGET)
		CLEAN_OBJS = $(OBJFILES)
endif

all: $(TARGET)

$(TARGET_BIN): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET_BIN) $(OBJFILES) $(LDFLAGS)

obj/%.o: src/%.c
	@$(MKDIR)
	$(CC) $(CFLAGS) $(CURSES_INCLUDE) -c $< -o $@

clean:
	@$(RM) $(CLEAN_OBJS) $(TARGET_BIN) *~ 2>cfg_err || true
	@$(RMDIR) 2>cfg_err || true

.PHONY: all clean
