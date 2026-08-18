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
		RMDIR = if exist obj rmdir /S /Q obj

		EXE = .exe
		CLEAN_OBJS = $(subst /,\\,$(OBJFILES))
else
    LDFLAGS = -lpthread -lm -ldl -lncursesw
    CURSES_INCLUDE =

		# set commands on unix
		MKDIR = mkdir -p obj
		RM = rm -f
		RMDIR = rm -rf obj
		EXE =
		CLEAN_OBJS = $(OBJFILES)
endif

OUTPUT: $(TARGET)$(EXE)

all: $(OUTPUT)

$(OUTPUT): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJFILES) $(LDFLAGS)

obj/%.o: src/%.c
	@$(MKDIR)
	$(CC) $(CFLAGS) $(CURSES_INCLUDE) -c $< -o $@

clean:
	@$(RM) $(CLEAN_OBJS) $(OUTPUT) *~ 2>nul || true
	@$(RMDIR) 2>nul || true

.PHONY: all clean
