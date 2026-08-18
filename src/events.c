#include "events.h"

void process_events(AppState *app) {
  int ch = getch();
  if (ch == ERR)
    return;

  // cmd mode
  if (app->cmd_mode) {
    int cmd_len = strlen(app->cmd_buf);

    // enter :commands here
    if (ch == '\n' || ch == KEY_ENTER) {

      char *cmd = strtok(app->cmd_buf, " ");
      char *arg = strtok(NULL, " ");

      if (cmd == NULL) {
        app->cmd_mode = false;
        memset(app->cmd_buf, 0, sizeof(app->cmd_buf));
        return;
      }

      // quit
      if (strcmp(cmd, "q") == 0) {
        app->is_running = false;
      }

      // to change dirlist width
      if (strcasecmp(cmd, "files") == 0) {
        if (arg != NULL) {
          errno = 0;
          long new_size = strtol(arg, NULL, 10);
          if (errno == 0) {
            app->dirlist_width = new_size;
            app->cmd_mode = false;
            memset(app->cmd_buf, 0, sizeof(app->cmd_buf));
            // reinit window size
            init_windows(app);
          } else {
            snprintf(app->cmd_buf, sizeof(app->cmd_buf), "%s",
                     "error setting dirlist width");
            app->cmd_mode = false;
          }
        } else {

          // assign buffer to the dirlist width value which renders in statusbar
          snprintf(app->cmd_buf, sizeof(app->cmd_buf), "%d",
                   app->dirlist_width);
          app->cmd_mode = false;
        }
      }
    }
    // ESC key (close cmd mode)
    else if (ch == 27) {
      app->cmd_mode = false;
      memset(app->cmd_buf, 0, sizeof(app->cmd_buf));
    }
    // backspace
    else if ((ch == KEY_BACKSPACE || ch == 127)) {
      if (cmd_len > 0) {
        app->cmd_buf[cmd_len - 1] = '\0';
      }
      // if no command, backspace closes command mode
      else {
        app->cmd_mode = false;
      }
    } else if (ch >= 32 && ch < 127 &&
               cmd_len < (int)sizeof(app->cmd_buf) - 1) {
      app->cmd_buf[cmd_len] = (char)ch;
    }
    return;
  }

  // help
  if (app->selected_window == HELP) {
    switch (ch) {
    case 'j':
    case KEY_DOWN:
      app->help_scroll++;
      break;
    case 'k':
    case KEY_UP:
      app->help_scroll--;
      break;
    }
  }

  // dir list
  int total = app->cwd_dir_list->length + app->cwd_song_list->length;
  if (app->selected_window == DIRLIST) {
    switch (ch) {
    case 'k':
    case KEY_UP:
      if (app->dir_selected_idx > 0)
        app->dir_selected_idx--;
      break;
    case 'j':
    case KEY_DOWN:
      if (app->dir_selected_idx < total - 1)
        app->dir_selected_idx++;
      break;
    case '\n':
    case KEY_ENTER: {
      int idx = app->dir_selected_idx;
      if (idx < (int)app->cwd_dir_list->length) {
        // it's a dir — cd into it
        const char *dir = (const char *)app->cwd_dir_list->data[idx];
        change_directory(app, dir);
      } else {
        // it's a song — add to queue
        Song *song =
            (Song *)app->cwd_song_list->data[idx - app->cwd_dir_list->length];
        load_to_queue(app, song);

        // if it is the first song added to empty queue set as the current
        if (app->queue->size == 1) {
          app->current = app->queue->head;
          app->queue_selected_idx = 0;
        }
      }
      break;
    }
    case CTRL('a'): {
      // add all songs in the song_list to queue
      for (int i = 0; i < app->cwd_song_list->length; i++) {
        Song *song = (Song *)app->cwd_song_list->data[i];
        load_to_queue(app, song);
        if (app->queue->size == 1) {
          app->current = app->queue->head;
          app->queue_selected_idx = 0;
        }
      }
      break;
    }
    }
  }

  // queue
  if (app->selected_window == QUEUE) {
    switch (ch) {
    case 'j':
    case KEY_DOWN:
      if (app->queue_selected_idx < app->queue->size - 1)
        app->queue_selected_idx++;
      break;
    case 'k':
    case KEY_UP:
      if (app->queue_selected_idx > 0)
        app->queue_selected_idx--;
      break;
    case '\n':
    case KEY_ENTER: {
      // traverse to selected idx
      Song *song = app->queue->head;
      for (int i = 0; i < app->queue_selected_idx && song != NULL; i++) {
        song = song->next;
      }
      // play sound
      if (song != NULL) {
        if (app->current != NULL)
          ma_sound_stop(&app->current->sound);
        app->current = song;
        ma_sound_seek_to_pcm_frame(&app->current->sound, 0);
        ma_sound_start(&app->current->sound);
        app->is_playing = true;
      }
      break;
    }
    case 'r': {
      // remove selected song from queue
      Song *song = app->queue->head;
      for (int i = 0; i < app->queue_selected_idx && song != NULL; i++) {
        song = song->next;
      }
      if (song != NULL) {
        if (song == app->current) {
          app->current = song->next;
          app->is_playing = false;
        }
        queue_remove(app->queue, song);
        if (app->queue_selected_idx > app->queue->size - 1) {
          app->queue_selected_idx = app->queue->size - 1;
        }
      }
      break;
    }
    case CTRL('r'): {
      // clear the queue
      queue_clear(app->queue);
      // reset
      app->current = NULL;
      app->is_playing = false;
      break;
    }
    case 'K':
    case KEY_SR:
      // move song up in queue
      queue_shift_up(app->queue, app->queue_selected_idx);
      if (app->queue_selected_idx > 0)
        app->queue_selected_idx--;
      break;
    case 'J':
    case KEY_SF:
      // move song down in queue
      queue_shift_down(app->queue, app->queue_selected_idx);
      if (app->queue_selected_idx < app->queue->size - 1)
        app->queue_selected_idx++;
      break;
    }
  }

  // player controls (still global)
  switch (ch) {
  case ' ':
    if (app->current != NULL) {
      if (app->is_playing) {
        ma_sound_stop(&app->current->sound);
        app->is_playing = false;
      } else {
        ma_sound_start(&app->current->sound);
        app->is_playing = true;
      }
    }
    break;
  case '+':
    if (app->volume < 100) {
      app->volume++;
      ma_engine_set_volume(app->engine, app->volume / 100.0f);
    }
    break;
  case '-':
    if (app->volume > 0) {
      app->volume--;
      ma_engine_set_volume(app->engine, app->volume / 100.0f);
    }
    break;
  case 'n': // next
    if (app->current != NULL && app->current->next != NULL) {
      ma_sound_stop(&app->current->sound);
      app->current = app->current->next;
      ma_sound_seek_to_pcm_frame(&app->current->sound, 0);
      ma_sound_start(&app->current->sound);
      app->is_playing = true;
    }
    break;
  case 'p': // prev
    if (app->current != NULL && app->current->prev != NULL) {
      ma_sound_stop(&app->current->sound);
      app->current = app->current->prev;
      ma_sound_seek_to_pcm_frame(&app->current->sound, 0);
      ma_sound_start(&app->current->sound);
      app->is_playing = true;
    }
    break;
  case '<': // seek backwards
    if (app->current != NULL) {
      ma_uint64 cursor;
      ma_sound_get_cursor_in_pcm_frames(&app->current->sound, &cursor);
      ma_uint32 sample_rate;
      ma_sound_get_data_format(&app->current->sound, NULL, NULL, &sample_rate,
                               NULL, 0);
      ma_uint64 seek_amount = sample_rate * 5;
      ma_uint64 new_pos = cursor > seek_amount ? cursor - seek_amount : 0;
      ma_sound_seek_to_pcm_frame(&app->current->sound, new_pos);
    }
    break;
  case '>': // seek forwards
    if (app->current != NULL) {
      ma_uint64 cursor;
      ma_sound_get_cursor_in_pcm_frames(&app->current->sound, &cursor);
      ma_uint32 sample_rate;
      ma_sound_get_data_format(&app->current->sound, NULL, NULL, &sample_rate,
                               NULL, 0);
      ma_uint64 length;
      ma_sound_get_length_in_pcm_frames(&app->current->sound, &length);
      ma_uint64 seek_amount = sample_rate * 5;
      ma_uint64 new_pos = cursor + seek_amount;
      // prevent ending the track seeking past the song's duration
      ma_uint64 max_pos =
          length > (ma_uint64)sample_rate ? length - sample_rate : 0;
      if (new_pos > max_pos)
        new_pos = max_pos;
      ma_sound_seek_to_pcm_frame(&app->current->sound, new_pos);
    }
    break;
  }

  // global
  switch (ch) {
  case ':':
    app->cmd_mode = true;
    memset(app->cmd_buf, 0, sizeof(app->cmd_buf));
    break;
  case '?':
    app->selected_window = app->selected_window == HELP ? DIRLIST : HELP;
    break;
  case '\t': {
    static const Selected cycle[] = {DIRLIST, QUEUE};
    static const int ncycle = 2;
    if (app->selected_window == HELP) {
      app->selected_window = cycle[0];
      break;
    }
    int cur = 0;
    for (int i = 0; i < ncycle; i++)
      if (cycle[i] == app->selected_window) {
        cur = i;
        break;
      }
    app->selected_window = cycle[(cur + 1) % ncycle];
    break;
  }
  case KEY_BTAB: {
    static const Selected cycle[] = {DIRLIST, QUEUE};
    static const int ncycle = 2;
    if (app->selected_window == HELP) {
      app->selected_window = cycle[ncycle - 1];
      break;
    }
    int cur = 0;
    for (int i = 0; i < ncycle; i++)
      if (cycle[i] == app->selected_window) {
        cur = i;
        break;
      }
    app->selected_window = cycle[(cur - 1 + ncycle) % ncycle];
    break;
  }
  case KEY_RESIZE:
    // TODO: add more implementation for resizing terminal
    resizeterm(0, 0);
    init_windows(app);
    clear();
    refresh();
    break;
  default:
    break;
  }
}
