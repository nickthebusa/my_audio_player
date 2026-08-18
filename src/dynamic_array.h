#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <stdio.h>

#define INITIAL_CAPACITY 64

typedef struct DynamicArray {
  void **data;
  int length;
  int capacity;
} DynamicArray;

void dynamic_array_init(DynamicArray *array);

void dynamic_array_push(DynamicArray *array, void *item);

void *dynamic_array_get(DynamicArray *array, int index);

void dynamic_array_free(DynamicArray *array, void (*free_item)(void *));

#endif
