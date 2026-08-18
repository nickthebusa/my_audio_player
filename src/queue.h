#ifndef QUEUE_H
#define QUEUE_H

#include "song.h"

typedef struct Queue {
  int size;
  Song *head;
  Song *tail;
} Queue;

void queue_init(Queue *queue);

void queue_push(Queue *queue, Song *song);

void queue_insert(Queue *queue, Song *song, int index);

void queue_remove(Queue *queue, Song *song);

Song *queue_pop(Queue *queue);

void queue_clear(Queue *queue);

void queue_shift_up(Queue *queue, int index);

void queue_shift_down(Queue *queue, int index);

void queue_free(Queue *queue);

#endif
