#ifndef _DArray_h
#define _DArray_h
#include <assert.h>
#include <lcthw/dbg.h>
#include <stdlib.h>

// clang-format off
typedef struct DArray {
    int end;                  // The current number of elements in the array (index of the next free slot).
    int max;                  // The maximum number of elements the array can hold before expanding.
    size_t element_size;      // The size of each element in the array (in bytes).
    size_t expand_rate;       // The rate at which the array grows when more space is needed.
    void **contents;          // A pointer to the actual storage for the array elements (array of void pointers).
} DArray;
// clang-format on

DArray *DArray_create(size_t element_size, size_t initial_max);

void DArray_destroy(DArray *array);

void DArray_clear(DArray *array);

int DArray_expand(DArray *array);

int DArray_contract(DArray *array);

int DArray_push(DArray *array, void *el);

void *DArray_pop(DArray *array);

void DArray_clear_destroy(DArray *array);

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline void DArray_set(DArray *array, int i, void *el) {
  check(i < array->max, "darray attempt to set past max");
  if (i > array->end)
    array->end = i;
  array->contents[i] = el;
error:
  return;
}

static inline void *DArray_get(DArray *array, int i) {
  check(i < array->max, "darray attempt to get past max");
  return array->contents[i];
error:
  return NULL;
}

static inline void *DArray_remove(DArray *array, int i) {
  void *el = array->contents[i];

  array->contents[i] = NULL;

  return el;
}

static inline void *DArray_new(DArray *array) {
  check(array->element_size > 0, "Can't use DArray_new on 0 size darrays.");

  return calloc(1, array->element_size);

error:
  return NULL;
}

#define DArray_free(E) free((E))

#define DArray_for_each(Arr, Ty, E)                                                \
  for (int i = 0; i < DArray_end(Arr); i++)                                    \
    for (Ty E = (Ty)DArray_get(Arr, i); E != NULL; E = NULL)

#endif
