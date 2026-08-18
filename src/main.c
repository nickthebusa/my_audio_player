#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "miniaudio.h"
#include "tinydir.h"

// my files
#include "dynamic_array.h"
#include "events.h"
#include "main.h"
#include "queue.h"
#include "render.h"
#include "update.h"

int main() {

  setlocale(LC_ALL, ""); // required for wide char support

  // curses init
  initscr();             // Start curses mode
  raw();                 // Line buffering disabled
  keypad(stdscr, TRUE);  // We get F1, F2 etc..
  noecho();              // Don't echo() while we do getch
  nodelay(stdscr, TRUE); // Removes blocking on getting input
  curs_set(0);           // invisible cursor

  // create color pairs
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);  // selected
  init_pair(2, COLOR_WHITE, COLOR_BLACK); // regular

  // miniaudio init
  ma_result result;
  ma_engine engine;
  ma_engine_config audio_config;

  // config audio engine
  audio_config = ma_engine_config_init();
  // change audio_config below
  // (using defaults for now)

  result = ma_engine_init(&audio_config, &engine);
  if (result != MA_SUCCESS) {
    printf("Error initializing miniaudio");
    return -1;
  }

  /* ----- start main ----- */

  AppState app;
  app.engine = &engine;
  bool res = app_init(&app);
  if (!res) {
    app_cleanup(&app);
  }

  // scan from cwd down (./)
  tinydir_open(&app.dir, ".");
  // adds folders and files to the cwd list
  scan_directory(&app);

  // main loop
  while (app.is_running) {
    process_events(&app);
    update(&app);
    render_main(&app);

    // 60fps, ncurses wrapper around nanosleep
    napms(16); 
  }

  /* --- end of program --- */
  app_cleanup(&app);

  return 0;
}

// checks file extension, returns true if it is an audio file
bool check_audio_extension(char *file_name) {
  char *ext = strrchr(file_name, '.');

  if (ext == NULL)
    return false;

  return strcmp(ext, ".mp3") == 0 || strcmp(ext, ".wav") == 0 ||
         strcmp(ext, ".flac") == 0;
}

// loads audio file and returns true if successful
bool load_audio_file(ma_engine *engine, char *file_name, ma_sound *sound) {
  ma_result res;
  res = ma_sound_init_from_file(engine, file_name, MA_SOUND_FLAG_STREAM, NULL,
                                NULL, sound);
  if (res != MA_SUCCESS) {
    printf("Error loading audio file %s", file_name);
    return false;
  }
  return true;
}

// scans the app->dir for directories and files
void scan_directory(AppState *app) {
  int row = 0;

  // make sure the parent dir is at the top of the list
  char *dir = malloc(strlen("..") + 1);
  if (dir == NULL) {
    fprintf(stderr, "Error allocating directory name\n");
    return;
  }
  strcpy(dir, "..");
  dynamic_array_push(app->cwd_dir_list, dir);

  while (app->dir.has_next) {
    tinydir_file file;
    tinydir_readfile(&app->dir, &file);

    if (file.is_dir) {

      // don't include the  current (./) and parent (../) directories
      // (already added parent)
      if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) {
        tinydir_next(&app->dir);
        continue;
      }

      char *dir = malloc(strlen(file.name) + 1);
      if (dir == NULL) {
        fprintf(stderr, "Error allocating directory name\n");
        continue;
      }
      strcpy(dir, file.name);
      dynamic_array_push(app->cwd_dir_list, dir);
      row++;
    } else {
      // check if file is audio
      if (check_audio_extension(file.name)) {
        // create song (not loaded yet)
        Song *song = malloc(sizeof(Song));
        if (song == NULL) {
          fprintf(stderr, "Error allocating song\n");
          continue;
        }
        // set file path (absolute) (manually add null terminator to prevent UB)
        strncpy(song->path, file.path, sizeof(song->path) - 1);
        song->path[sizeof(song->path) - 1] = '\0';

        // set title
        strncpy(song->title, file.name, sizeof(song->title) - 1);
        song->title[sizeof(song->title) - 1] = '\0';

        song->is_loaded = false;
        song->next = NULL;
        song->prev = NULL;

        // TODO: add metadata

        dynamic_array_push(app->cwd_song_list, song);

        row++;
      }
    }

    // get next file/dir and increment row
    tinydir_next(&app->dir);
  }
}

// switches the current directory for directory explorer
void change_directory(AppState *app, const char *path) {
  // use path to change directory
  // after freeing the lists path is junk values
  chdir(path);

  // free dir list
  for (int i = 0; i < (int)app->cwd_dir_list->length; i++)
    free(app->cwd_dir_list->data[i]);
  app->cwd_dir_list->length = 0;

  // free song list
  for (int i = 0; i < (int)app->cwd_song_list->length; i++)
    free(app->cwd_song_list->data[i]);
  app->cwd_song_list->length = 0;

  // reset selection
  app->dir_selected_idx = 0;

  // close current and open in the newly selected directory
  tinydir_close(&app->dir);
  tinydir_open(&app->dir, ".");
  scan_directory(app);
}

bool app_init(AppState *app) {
  // set state
  app->is_running = true;
  app->is_playing = false;
  app->current = NULL;
  app->selected_window = DIRLIST;
  app->volume = 80;
  app->dirlist_width = DIRLIST_INIT_WIDTH;
  app->cmd_mode = false;
  app->dir_selected_idx = 0;
  app->queue_selected_idx = 0;
  app->help_scroll = 0;

  // init volume
  ma_engine_set_volume(app->engine, app->volume / 100.0f);

  // init queue
  app->queue = malloc(sizeof(Queue));
  if (app->queue == NULL) {
    fprintf(stderr, "Error allocating queue\n");
    return false;
  }
  queue_init(app->queue);

  // init cwd song list
  app->cwd_song_list = malloc(sizeof(DynamicArray));
  if (app->cwd_song_list == NULL) {
    fprintf(stderr, "Error allocating cwd list\n");
    return false;
  }
  dynamic_array_init(app->cwd_song_list);

  // init cwd dir list
  app->cwd_dir_list = malloc(sizeof(DynamicArray));
  if (app->cwd_dir_list == NULL) {
    fprintf(stderr, "Error allocating cwd list\n");
    return false;
  }
  dynamic_array_init(app->cwd_dir_list);

  // init windows
  init_windows(app);

  return true;
}

void app_cleanup(AppState *app) {
  // free the data structure items
  // then free the struct itself

  // current song is also freed here
  if (app->queue != NULL) {
    queue_free(app->queue);
    free(app->queue);
  }

  if (app->cwd_song_list != NULL) {
    dynamic_array_free(app->cwd_song_list, NULL);
    free(app->cwd_song_list);
  }

  if (app->cwd_dir_list != NULL) {
    dynamic_array_free(app->cwd_dir_list, NULL);
    free(app->cwd_dir_list);
  }

  uninit_windows(app);

  tinydir_close(&app->dir);

  ma_engine_uninit(app->engine);

  // End curses mode
  endwin();
}

void init_windows(AppState *app) {
  // clear old windows when resizing
  uninit_windows(app);

  int total_rows = LINES;
  int total_cols = COLS;

  int queue_height = total_rows - PLAYER_HEIGHT - STATUSBAR_HEIGHT;
  int queue_width = total_cols - app->dirlist_width;
  int dirlist_height = total_rows - STATUSBAR_HEIGHT;
  // overlay help window dimensions
  int height = 24;
  int width = 70;
  int start_y = (LINES - height) / 2;
  int start_x = (COLS - width) / 2;

  // create windows:  newwin(height, width, start_row, start_col)
  app->win_dirlist = newwin(dirlist_height, app->dirlist_width, 0, 0);
  app->win_queue = newwin(queue_height, queue_width, 0, app->dirlist_width);
  app->win_player =
      newwin(PLAYER_HEIGHT, queue_width, queue_height, app->dirlist_width);
  app->win_statusbar =
      newwin(STATUSBAR_HEIGHT, total_cols, total_rows - STATUSBAR_HEIGHT, 0);
  app->win_help = newwin(height, width, start_y, start_x);
}

// deletes and sets to NULL
void uninit_windows(AppState *app) {
  if (app->win_dirlist) {
    delwin(app->win_dirlist);
    app->win_dirlist = NULL;
  }
  if (app->win_queue) {
    delwin(app->win_queue);
    app->win_queue = NULL;
  }
  if (app->win_player) {
    delwin(app->win_player);
    app->win_player = NULL;
  }
  if (app->win_statusbar) {
    delwin(app->win_statusbar);
    app->win_statusbar = NULL;
  }
  if (app->win_help) {
    delwin(app->win_help);
    app->win_help = NULL;
  }
}

// actually loads song data and adds to queue
void load_to_queue(AppState *app, Song *song) {
  Song *copy = malloc(sizeof(Song));
  if (copy == NULL) {
    fprintf(stderr, "Error allocating song\n");
    return;
  }
  memcpy(copy, song, sizeof(Song));
  copy->next = NULL;
  copy->prev = NULL;

  // load the track
  if (ma_sound_init_from_file(app->engine, copy->path, 0, NULL, NULL,
                              &copy->sound) != MA_SUCCESS) {
    fprintf(stderr, "Error loading song: %s\n", copy->path);
    free(copy);
    return;
  }
  copy->is_loaded = true;

  copy->duration = get_total_time(copy);

  queue_push(app->queue, copy);
}

void format_time(float seconds, char *buf, int buf_size) {
  int mins = (int)seconds / 60;
  int secs = (int)seconds % 60;
  snprintf(buf, buf_size, "%02d:%02d", mins, secs);
}

