#ifndef MAIN_H
#define MAIN_H

#ifdef _WIN32
#include "pdcursesmod/curses.h"
#else
#include <ncurses.h>
#endif

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include <errno.h>
#include <string.h>

#include "miniaudio.h"
#include "tinydir.h"
#include <stdbool.h>

#include "dynamic_array.h"
#include "queue.h"

#define DIRLIST_INIT_WIDTH 25
#define PLAYER_HEIGHT 3
#define STATUSBAR_HEIGHT 1

typedef enum Selected {
  DIRLIST,
  QUEUE,
  HELP,
} Selected;

typedef struct AppState {
  ma_engine *engine;
  tinydir_dir dir;
  int dir_selected_idx;
  int queue_selected_idx;
  bool is_running;
  bool is_playing;
  int volume;
  Queue *queue;
  Song *current;
  DynamicArray *cwd_song_list;
  DynamicArray *cwd_dir_list;
  Selected selected_window;
  char cmd_buf[64];
  bool cmd_mode;
  int dirlist_width;
  int help_scroll;

  // window panels
  WINDOW *win_dirlist;
  WINDOW *win_queue;
  WINDOW *win_player;
  WINDOW *win_statusbar;
  WINDOW *win_help;
} AppState;

bool check_audio_extension(char *file_name);

bool load_audio_file(ma_engine *engine, char *file_name, ma_sound *sound);

void scan_directory(AppState *app);

void change_directory(AppState *app, const char *path);

bool app_init(AppState *app);

void init_windows(AppState *app);

void uninit_windows(AppState *app);

void app_cleanup(AppState *app);

void load_to_queue(AppState *app, Song *song);

void format_time(float seconds, char *buf, int buf_size);
#endif
