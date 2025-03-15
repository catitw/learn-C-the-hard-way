#ifndef lcthw_List_h
#define lcthw_List_h

#include <stdlib.h>

struct ListNode;

typedef struct ListNode {
  struct ListNode *next;
  struct ListNode *prev;
  void *value;
} ListNode;

// NOTE: if `first` or `last` is `NULL`, means the list is empty.
typedef struct List {
  int count;
  ListNode *first; // Pointer to the first node in the list
  ListNode *last;  // Pointer to the last node in the list
} List;

/**
 * Creates a new, empty doubly linked list.
 *
 * @return A pointer to the newly created list.
 */
List *List_create();

/**
 * Destroys the given list and frees all associated memory,
 * but the memory of values will not be freed.
 *
 * @param list A pointer to the list to destroy.
 */
void List_destroy(List *list);

/**
 * Clears all the values stored in the nodes of the list,
 * but does not destroy the list itself.
 *
 * @param list A pointer to the list to clear.
 */
void List_clear(List *list);

/**
 * Clears all the values in the list and destroys the list,
 * freeing all associated memory.
 *
 * @param list A pointer to the list to clear and destroy.
 */
void List_clear_destroy(List *list);

#define List_count(A) ((A)->count)

#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)

#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

/**
 * Adds a new value to the end of the list.
 *
 * @param list A pointer to the list.
 * @param value The value to add to the list.
 */
void List_push(List *list, void *value);

/**
 * Removes and returns the value at the end of the list.
 *
 * @param list A pointer to the list.
 * @return The value removed from the end of the list.
 */
void *List_pop(List *list);

/**
 * Adds a new value to the beginning of the list.
 *
 * @param list A pointer to the list.
 * @param value The value to add to the list.
 */
void List_unshift(List *list, void *value);

/**
 * Removes and returns the value at the beginning of the list.
 *
 * @param list A pointer to the list.
 * @return The value removed from the beginning of the list.
 */
void *List_shift(List *list);

/**
 * Removes the specified node from the list and returns its value.
 *
 * @param list A pointer to the list.
 * @param node A pointer to the node to remove.
 * @return The value of the removed node.
 */
void *List_remove(List *list, ListNode *node);

/**
 * @note the val is shallow copied.
 */
List *List_duplicate(List *list);

/**
 * @note the val is shallow copied.
 */
List *List_concat(List *head, List *tail);

/**
 * @note the val is shallow copied.
 */
void List_split(List *list, ListNode *new_head_after_spilit, List **head_ret,
                List **tail_ret);
/**
 * Macro to iterate over the elements of the list.
 *
 * @param L A pointer to the list.
 * @param S The starting point for iteration (e.g., `first` or `last`).
 * @param M The direction of iteration (e.g., `next` or `prev`).
 * @param V A variable to hold the current node during iteration.
 *
 * Example usage:
 * ```
 * LIST_FOREACH(my_list, first, next, current_node) {
 *     printf("%s\n", (char *)current_node->value);
 * }
 * ```
 */
#define LIST_FOREACH(L, S, M, V)                                               \
  ListNode *_node = NULL;                                                      \
  ListNode *V = NULL;                                                          \
  for (V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif
