#ifndef RENDER_H
#define RENDER_H

#ifdef _WIN32
#include "pdcurses/curses.h"
#else
#include <ncurses.h>
#endif

#include "main.h"
#include "dynamic_array.h"

#define TIME_BUFFER_SIZE 10

void render_main(AppState* app);

void render_directory_list(AppState* app);

void render_queue(AppState* app);

void render_player(AppState* app);

void render_statusbar(AppState *app);

void render_help(AppState* app);
#endif
