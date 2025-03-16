#ifndef lcthw_Stack_h
#define lcthw_Stack_h

#include <lcthw/darray.h>

#define STACK_DEFAULT_CAPACITY 100

typedef struct Stack {
  DArray *darray;
} Stack;

static inline Stack *Stack_create() {
  Stack *stack = calloc(1, sizeof(Stack));

  stack->darray = DArray_create(sizeof(void *), STACK_DEFAULT_CAPACITY);

  return stack;
}

static inline void Stack_destroy(Stack *stack) {
  DArray_clear_destroy(stack->darray);
  free(stack);
}

static inline void Stack_push(Stack *stack, void *value) {
  DArray_push(stack->darray, value);
}

static inline void *Stack_pop(Stack *stack) {
  return DArray_pop(stack->darray);
}

static inline void *Stack_peek(Stack *stack) {
  return DArray_last(stack->darray);
}

static inline int Stack_count(Stack *stack) {
  return DArray_count(stack->darray);
}

#define STACK_FOREACH(S, Ty, V) DArray_for_each(S->darray, Ty, V)

#endif