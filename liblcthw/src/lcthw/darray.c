#include "lcthw/dbg.h"
#include <lcthw/darray.h>
#include <string.h>

// Create a new dynamic array
DArray *DArray_create(size_t element_size, size_t initial_max) {
  DArray *array = NULL;

  check(initial_max >= 1, "Initial max must be at least 1");

  check(DEFAULT_EXPAND_RATE > 0, "Invalid expand rate: %d",
        DEFAULT_EXPAND_RATE);

  array = malloc(sizeof(DArray));
  check(array != NULL, "Memory allocation failed for DArray.");

  array->end = 0;
  array->max = initial_max;
  array->element_size = element_size;
  array->expand_rate = DEFAULT_EXPAND_RATE;

  array->contents = calloc(initial_max, sizeof(void *));
  check(array->contents != NULL,
        "Memory allocation failed for DArray contents.");

  return array;

error:
  if (array)
    free(array);
  return NULL;
}

// Destroy the dynamic array and free its memory
void DArray_destroy(DArray *array) {
  if (array) {
    free(array->contents);
    free(array);
  }
}

// Clear the contents of the dynamic array (but keep its structure)
void DArray_clear(DArray *array) {
  if (array->contents) {
    for (int i = 0; i < array->max; i++) {
      if (array->contents[i]) {
        DArray_free(array->contents[i]);
      }
    }
  }

  array->end = 0;
}

// Expand the dynamic array when needed
int DArray_expand(DArray *array) {
  size_t old_max = array->max;
  array->max += array->expand_rate;

  void **contents = realloc(array->contents, array->max * sizeof(void *));
  check(contents != NULL, "Failed to expand DArray.");

  array->contents = contents;

  // initialize newly allocated memory
  memset(array->contents + old_max, 0, array->expand_rate * sizeof(void *));
  return 0;

error:
  return -1;
}

// Contract the dynamic array to save memory
int DArray_contract(DArray *array) {
  // keep at least the expand rate amount of slots
  int new_size = (size_t)array->end < array->expand_rate + 1
                     ? array->expand_rate + 1
                     : (size_t)array->end;

  if (new_size < array->max) {
    void **contents = realloc(array->contents, new_size * sizeof(void *));
    check(contents != NULL, "Failed to contract DArray.");

    array->contents = contents;
    array->max = new_size;
  }

  return 0;

error:
  return -1;
}

// Push an element onto the dynamic array
//
// NOTE: The invariant is that a array must have at least one empty slot after
// created.
int DArray_push(DArray *array, void *el) {
  array->contents[array->end] = el;
  array->end++;

  if (array->end >= array->max) {
    return DArray_expand(array);
  } else {
    return 0;
  }
}

// Pop an element off the dynamic array
void *DArray_pop(DArray *array) {
  check(array->end > 0, "Attempt to pop from empty array.");

  void *el = DArray_remove(array, array->end - 1);
  array->end--;

  return el;

error:
  return NULL;
}

// Clear and destroy the dynamic array
void DArray_clear_destroy(DArray *array) {
  DArray_clear(array);
  DArray_destroy(array);
}
