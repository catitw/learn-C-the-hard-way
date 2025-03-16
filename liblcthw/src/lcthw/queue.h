#ifndef lcthw_Queue_h
#define lcthw_Queue_h

#include "lcthw/list.h"
typedef struct Queue {
  List *list;
} Queue;

static inline Queue *Queue_create() {
  Queue *queue = malloc(sizeof(Queue));
  queue->list = List_create();
  return queue;
}

static inline void Queue_destroy(Queue *queue) {
  List_clear_destroy(queue->list);
  free(queue);
}

static inline void Queue_send(Queue *queue, void *value) {
  List_push(queue->list, value);
}

/**
 * Removes and returns the value at the front of the queue.
 */
static inline void *Queue_recv(Queue *queue) { return List_shift(queue->list); }

static inline void *Queue_peek(Queue *queue) { return List_first(queue->list); }

static inline int Queue_count(Queue *queue) { return List_count(queue->list); }

#define QUEUE_FOREACH(Q, V) LIST_FOREACH((Q)->list, first, next, V)

#endif