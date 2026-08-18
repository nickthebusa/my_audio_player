#include "queue.h"
#include <stdlib.h>

void queue_init(Queue *queue) {
  queue->size = 0;
  queue->head = NULL;
  queue->tail = NULL;
}

void queue_push(Queue *queue, Song *song) {
  // init song pointers
  song->next = NULL;
  song->prev = NULL;
  // if queue is empty
  if (queue->head == NULL) {
    queue->head = song;
    queue->tail = song;
  } else {
    queue->tail->next = song;
    song->prev = queue->tail;
    queue->tail = song;
  }
  queue->size++;
}

void queue_insert(Queue *queue, Song *song, int index) {
  if (queue->size >= index) {
    queue_push(queue, song);
    return;
  }

  if (index == 0) {
    song->next = queue->head;
    song->prev = NULL;
    if (queue->head != NULL) {
      queue->head->prev = song;
    }
    queue->head = song;
    if (queue->tail == NULL) {
      queue->tail = song;
    }
    queue->size++;
    return;
  }

  // get the node at the current idx
  Song *cur = queue->head;
  for (int i = 0; i < index && cur != NULL; i++) {
    cur = cur->next;
  }
  if (cur == NULL)
    return;

  // point the new node to the existing points
  song->next = cur;
  song->prev = cur->prev;
  cur->prev->next = song;
  cur->prev = song;
  queue->size++;
}

void queue_remove(Queue *queue, Song *song) {
  if (song == NULL)
    return;

  if (song->prev != NULL) {
    song->prev->next = song->next;
  } else {
    queue->head = song->next;
  }

  if (song->next != NULL) {
    song->next->prev = song->prev;
  } else {
    queue->tail = song->prev;
  }

  queue->size--;
  ma_sound_stop(&song->sound);
  ma_sound_uninit(&song->sound);
  free(song);
}

Song *queue_pop(Queue *queue) {
  if (queue->head == NULL)
    return NULL;

  Song *song = queue->head;
  queue->head = queue->head->next;
  if (queue->head != NULL) {
    queue->head->prev = NULL;
  } else {
    queue->tail = NULL;
  }
  song->next = NULL;
  song->prev = NULL;
  queue->size--;
  return song;
}

void queue_clear(Queue *queue) {
  while (queue->head != NULL) {
    Song *to_clear = queue_pop(queue);
    ma_sound_stop(&to_clear->sound);
    ma_sound_uninit(&to_clear->sound);
    free(to_clear);
  }
}

void queue_shift_up(Queue *queue, int index) {
  if (index <= 0)
    return;

  // traverse to selected node
  Song *song = queue->head;
  for (int i = 0; i < index && song != NULL; i++)
    song = song->next;

  if (song == NULL || song->prev == NULL)
    return;

  Song *prev = song->prev;

  // fix outer pointers
  if (prev->prev != NULL)
    prev->prev->next = song;
  else
    queue->head = song;

  if (song->next != NULL)
    song->next->prev = prev;
  else
    queue->tail = prev;

  // swap the two nodes
  song->prev = prev->prev;
  prev->next = song->next;
  song->next = prev;
  prev->prev = song;
}

void queue_shift_down(Queue *queue, int index) {
    if (index >= queue->size - 1)
        return;

    // traverse to selected node
    Song *song = queue->head;
    for (int i = 0; i < index && song != NULL; i++)
        song = song->next;

    if (song == NULL || song->next == NULL)
        return;

    Song *next = song->next;

    // fix outer pointers
    if (song->prev != NULL)
        song->prev->next = next;
    else
        queue->head = next;

    if (next->next != NULL)
        next->next->prev = song;
    else
        queue->tail = song;

    // swap the two nodes
    next->prev = song->prev;
    song->next = next->next;
    next->next = song;
    song->prev = next;
}

void queue_free(Queue *queue) {
  Song *song = queue->head;
  while (song != NULL) {
    Song *next = song->next;
    free(song);
    song = next;
  }
}
