#include "dynamic_array.h"

void dynamic_array_init(DynamicArray *array) {
  array->capacity = INITIAL_CAPACITY;
  array->length = 0;
  array->data = malloc(sizeof(void *) * array->capacity);
}

void dynamic_array_push(DynamicArray *array, void *item) {
  if (array->length >= array->capacity) {
    array->capacity *= 2;
    void **buf = realloc(array->data, sizeof(void *) * array->capacity);
    if (buf == NULL) {
      fprintf(stderr, "realloc failed, unable to expand array\n");
      array->capacity /= 2;
      return;
    }
    array->data = buf;
  }
  // set the new item to the current length then increment
  array->data[array->length++] = item;
}

void *dynamic_array_get(DynamicArray *array, int index) {
  return array->data[index];
}

void dynamic_array_free(DynamicArray *array, void (*free_item)(void *)) {
  if (free_item != NULL) {
    for (int i = 0; i < array->length; i++) {
      free_item(array->data[i]);
    }
  }
  free(array->data);
}
