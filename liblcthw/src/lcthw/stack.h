#ifndef lcthw_Stack_h
#define lcthw_Stack_h

#include <lcthw/list.h>

typedef struct Stack {
  List *list;
} Stack;

static inline Stack *Stack_create() {
  Stack *stack = calloc(1, sizeof(Stack));

  stack->list = List_create();

  return stack;
}

static inline void Stack_destroy(Stack *stack) {
  List_clear_destroy(stack->list);
  free(stack);
}

static inline void Stack_push(Stack *stack, void *value) {
  List_push(stack->list, value);
}

static inline void *Stack_pop(Stack *stack) { return List_pop(stack->list); }

static inline void *Stack_peek(Stack *stack) { return List_last(stack->list); }

static inline int Stack_count(Stack *stack) { return List_count(stack->list); }

#define STACK_FOREACH(S, V)                                                    \
  for (ListNode *V = S->list->last; V != NULL; V = V->prev)

#endif