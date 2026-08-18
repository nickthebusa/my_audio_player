#ifndef SONG_H
#define SONG_H

#include <stdbool.h>
#include "miniaudio.h"

typedef struct Song {
  char path[512];
  char title[256];
  char artist[256];
  char album[256];
  ma_sound sound;
  bool is_loaded;
  float duration; // seconds
  struct Song* next;
  struct Song* prev;
} Song;

float get_current_time(Song *song);

float get_total_time(Song *song);

#endif
