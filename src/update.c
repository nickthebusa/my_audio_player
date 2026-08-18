#include "update.h"

void update(AppState *app) {
  if (app->current == NULL || !app->is_playing)
    return;

  // check if current has finished playing
  if (!ma_sound_is_playing(&app->current->sound)) {
    // go to next
    if (app->current->next != NULL) {
      app->current = app->current->next;
      ma_sound_seek_to_pcm_frame(&app->current->sound, 0);
      ma_sound_start(&app->current->sound);
      // update the queue UI idx
      app->queue_selected_idx++;
    } else {
      // nothing left, stop playing
      app->is_playing = false;
      app->current = NULL;
    }
  }
  // if playing
  else {
  }
}

