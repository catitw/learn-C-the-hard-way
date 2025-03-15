#include <lcthw/list.h>
#include <lcthw/list_algos.h>

int List_bubble_sort(List *list, List_compare cmp) {
  ListNode *loop_end = list->last;

  if (loop_end == NULL) {
    return 0;
  }

  for (; loop_end != NULL; loop_end = loop_end->prev) {

    for (ListNode *cur = list->first; cur != loop_end; cur = cur->next) {

      ListNode *next = cur->next;

      if (next != NULL) {
        if (cmp(cur->value, next->value) > 0) {
          void *tmp = cur->value;
          cur->value = next->value;
          next->value = tmp;
        }
      }
    }
  }

  return 0;
}

static List *merge_sorted_lists(List *left, List *right, List_compare cmp) {
  List *l = List_create();

  ListNode *left_cur = left->first;
  ListNode *right_cur = right->first;

  while (left_cur != NULL && right_cur != NULL) {

    if (cmp(left_cur->value, right_cur->value) < 0) {
      List_push(l, left_cur->value);
      left_cur = left_cur->next;
    } else {
      List_push(l, right_cur->value);
      right_cur = right_cur->next;
    }
  }

  while (left_cur != NULL) {
    List_push(l, left_cur->value);
    left_cur = left_cur->next;
  }

  while (right_cur != NULL) {
    List_push(l, right_cur->value);
    right_cur = right_cur->next;
  }

  return l;
}

List *List_merge_sort(List *list, List_compare cmp) {

  if (list->count <= 1) {
    return list;
  }

  List *left = List_create();
  List *right = List_create();

  int left_count = list->count / 2;

  for (ListNode *cur = list->first; cur != NULL; cur = cur->next) {
    if (left_count > 0) {
      List_push(left, cur->value);
      --left_count;
    } else {
      List_push(right, cur->value);
    }
  }

  List *left_sorted = List_merge_sort(left, cmp);
  List *right_sorted = List_merge_sort(right, cmp);

  return merge_sorted_lists(left_sorted, right_sorted, cmp);
}
