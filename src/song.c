#include "song.h"

// returns in seconds

float get_current_time(Song *song) {
  ma_uint64 cursor;
  ma_sound_get_cursor_in_pcm_frames(&song->sound, &cursor);
  ma_uint32 sample_rate;
  ma_sound_get_data_format(&song->sound, NULL, NULL, &sample_rate, NULL, 0);
  return (float)cursor / (float)sample_rate;
}

float get_total_time(Song *song) {
  ma_uint64 length;
  ma_sound_get_length_in_pcm_frames(&song->sound, &length);
  ma_uint32 sample_rate;
  ma_sound_get_data_format(&song->sound, NULL, NULL, &sample_rate, NULL, 0);
  return (float)length / (float)sample_rate;
}
