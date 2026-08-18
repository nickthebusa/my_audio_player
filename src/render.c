#include "render.h"

void render_main(AppState *app) {
  render_directory_list(app);
  render_queue(app);
  render_player(app);
  render_statusbar(app);
  render_help(app);
  doupdate();
}

void render_directory_list(AppState *app) {
  WINDOW *win = app->win_dirlist;

  // clear window
  werase(win);

  if (app->selected_window == DIRLIST) {
    wattron(win, COLOR_PAIR(1));
  } else {
    wattron(win, COLOR_PAIR(2));
  }
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(1));
  wattroff(win, COLOR_PAIR(2));

  mvwprintw(win, 0, 2, " Files ");

  int max_rows, max_cols;
  getmaxyx(win, max_rows, max_cols);
  int available = max_rows - 2;

  int total = app->cwd_dir_list->length + app->cwd_song_list->length;

  // scroll offset so selected item stays visible
  int offset = 0;
  if (app->dir_selected_idx >= available)
    offset = app->dir_selected_idx - available + 1;

  for (int i = 0; i < available; i++) {
    int list_idx = i + offset;
    if (list_idx >= total)
      break;

    const char *display;
    bool is_dir;

    if (list_idx < (int)app->cwd_dir_list->length) {
      display = (const char *)app->cwd_dir_list->data[list_idx];
      is_dir = true;
    } else {
      Song *song =
          (Song *)
              app->cwd_song_list->data[list_idx - app->cwd_dir_list->length];
      display = song->title;
      is_dir = false;
    }

    bool is_selected = (list_idx == app->dir_selected_idx);

    if (is_selected && app->selected_window == DIRLIST)
      wattron(win, A_REVERSE);

    // prefix dirs with / so user can tell them apart
    mvwprintw(win, i + 1, 2, "%*.*s", -(max_cols - 4), max_cols - 4,
              is_dir ? "/" : " ");
    mvwprintw(win, i + 1, 3, "%*.*s", -(max_cols - 5), max_cols - 5, display);

    if (is_selected)
      wattroff(win, A_REVERSE);
  }

  wnoutrefresh(win);
}

void render_queue(AppState *app) {
  WINDOW *win = app->win_queue;
  werase(win);

  if (app->selected_window == QUEUE)
    wattron(win, COLOR_PAIR(1));
  else
    wattron(win, COLOR_PAIR(2));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(1));
  wattroff(win, COLOR_PAIR(2));

  mvwprintw(win, 0, 2, " Queue (%d) ", app->queue->size);

  if (app->queue->size == 0) {
    mvwprintw(win, 1, 2, "Queue is empty");
    wrefresh(win);
    return;
  }

  int max_rows, max_cols;
  getmaxyx(win, max_rows, max_cols);
  int available = max_rows - 2;

  // scroll offset so selected item stays visible
  int offset = 0;
  if (app->queue_selected_idx >= available)
    offset = app->queue_selected_idx - available + 1;

  // advance to offset
  Song *song = app->queue->head;
  for (int i = 0; i < offset && song != NULL; i++)
    song = song->next;

  for (int i = 0; i < available && song != NULL; i++, song = song->next) {
    bool is_current = (song == app->current);
    bool is_selected = (app->selected_window == QUEUE &&
                        (i + offset) == app->queue_selected_idx);

    const char *display;
    if (song->title[0] != '\0') {
      display = song->title;
    } else {
      display = strrchr(song->path, '/');
      display = display ? display + 1 : song->path;
    }

    if (is_current)
      wattron(win, A_BOLD);
    if (is_selected)
      wattron(win, A_REVERSE);

    mvwprintw(win, i + 1, 2, "%*.*s", -(max_cols - 4), max_cols - 4, display);

    if (is_current)
      wattroff(win, A_BOLD);
    if (is_selected)
      wattroff(win, A_REVERSE);
  }

  wnoutrefresh(win);
}

void render_player(AppState *app) {
  WINDOW *win = app->win_player;
  werase(win);

  box(win, 0, 0);

  int max_rows, max_cols;
  getmaxyx(win, max_rows, max_cols);

  // current track label
  const char *track = "--";
  if (app->current != NULL) {
    if (app->current->title[0] != '\0')
      track = app->current->title;
    else {
      track = strrchr(app->current->path, '/');
      track = track ? track + 1 : app->current->path;
    }
  }

  // truncate track name to fit between borders
  int track_max = max_cols - 4;
  mvwprintw(win, 1, 2, "%*.*s", -track_max, track_max, track);

  // controls: prev, play/pause, next
  const char *state = app->is_playing ? "[  ||  ]" : "[  >>  ]";
  int controls_x = (max_cols - 28) / 2;
  mvwprintw(win, 2, controls_x, "[  |<  ]  %s  [  >|  ]", state);

  // volume on the right
  mvwprintw(win, 2, max_cols - 12, "vol: %3d%%", app->volume);

  // time
  if (app->current != NULL) {
    float current_time = get_current_time(app->current);
    char cur_buf[TIME_BUFFER_SIZE];
    char length_buf[TIME_BUFFER_SIZE];
    format_time(current_time, cur_buf, TIME_BUFFER_SIZE);
    format_time(app->current->duration, length_buf, TIME_BUFFER_SIZE);
    mvwprintw(win, 0, (max_cols / 2) - 7, "%s / %s", cur_buf, length_buf);
  }

  wnoutrefresh(win);
}

void render_statusbar(AppState *app) {
  werase(app->win_statusbar);
  wattron(app->win_statusbar, A_REVERSE);
  mvwhline(app->win_statusbar, 0, 0, ' ', COLS);

  if (app->cmd_mode) {
    mvwprintw(app->win_statusbar, 0, 0, ":%s", app->cmd_buf);
  } else {
    const char *hint = "? for help";
    mvwprintw(app->win_statusbar, 0, 0, "%s", app->cmd_buf);
    mvwprintw(app->win_statusbar, 0, COLS - (int)strlen(hint) - 1, "%s", hint);
  }

  wattroff(app->win_statusbar, A_REVERSE);
  wnoutrefresh(app->win_statusbar);
}

void render_help(AppState *app) {
  if (app->selected_window != HELP)
    return;

  WINDOW *help = app->win_help;

  int height, width;
  getmaxyx(help, height, width);
  int start_y = (LINES - height) / 2;
  int start_x = (COLS - width) / 2;

  werase(help);
  mvwin(help, start_y, start_x);
  wattron(help, COLOR_PAIR(1));
  box(help, 0, 0);
  wattroff(help, COLOR_PAIR(1));
  mvwprintw(help, 0, 2, " Help ");

  int available = height - 2;

  const char *lines[] = {
      "Commands",
      "  :q                quit",
      "  :files            output file tree width",
      "  :files <width>    set file tree width",
      "  ?                 toggle help",
      "",
      "Player",
      "  Space             play / pause",
      "  n                 next track",
      "  p                 previous track",
      "  +                 volume up",
      "  -                 volume down",
      "  <                 seek song backwards",
      "  >                 seek song forwards",
      "",
      "Navigation",
      "  Tab / Shift-Tab   cycle windows",
      "  j / Down          move down",
      "  k / Up            move up",
      "",
      "Queue",
      "  ENTER             set and play selected",
      "  r                 remove from queue",
      "  CTRL+r            clear queue",
      "  Shift- k/up       move selected up",
      "  Shift- j/down     move selected down",
      "",
      "Files",
      "  ENTER             add selected to queue",
      "  CTRL+a            adds all of the songs in dir to queue",
      "",
      "Press TAB or ? to close",
  };
  int total_lines = sizeof(lines) / sizeof(lines[0]);

  // clamp scroll
  int max_scroll = total_lines - available;
  if (app->help_scroll < 0)
    app->help_scroll = 0;
  if (app->help_scroll > max_scroll)
    app->help_scroll = max_scroll;

  for (int i = 0; i < available; i++) {
    int line_idx = i + app->help_scroll;
    if (line_idx >= total_lines)
      break;
    mvwprintw(help, i + 1, 2, "%s", lines[line_idx]);
  }

  // scroll indicator on the right border
  if (total_lines > available) {
    mvwprintw(help, 1, width - 3, "^");
    mvwprintw(help, height - 2, width - 3, "v");
  }

  wnoutrefresh(help);
}
