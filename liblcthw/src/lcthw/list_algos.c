#include <lcthw/list.h>
#include <lcthw/list_algos.h>
#include <stdlib.h>

int List_bubble_sort(List *list, List_compare cmp) {
  ListNode *loop_end = list->last;

  if (loop_end == NULL) {
    return 0;
  }

  for (; loop_end != NULL; loop_end = loop_end->prev) {

    int swapped = 0;

    for (ListNode *cur = list->first; cur != loop_end; cur = cur->next) {

      ListNode *next = cur->next;

      if (next != NULL) {
        if (cmp(cur->value, next->value) > 0) {
          void *tmp = cur->value;
          cur->value = next->value;
          next->value = tmp;

          swapped = 1;
        }
      }
    }

    if (!swapped) {
      return 0;
    }
  }

  return 0;
}

// 相比于创建链表再排序：
// 减少了内存的申请和释放的开销
List *List_insert_sorted(List *sorted_list, ListNode *node, List_compare cmp) {

  // the val will be insert to the left of `insert_place`
  ListNode *right = sorted_list->first;

  for (; right != NULL; right = right->next) {
    if (cmp(right->value, node->value) > 0) {
      break;
    }
  }

  ListNode *left = right->prev;
  if (left == NULL) {
    // means the `right` is the first, and the `val` will be the first node.
    node->next = sorted_list->first;
    sorted_list->first->prev = node;
    sorted_list->first = node;
  } else {
    node->prev = left;
    node->next = right;
    left->next = node;
    right->prev = node;
  }

  sorted_list->count++;
  return sorted_list;
}

static List *merge_sorted_lists(List *left, List *right, List_compare cmp) {
  List *l = List_create();

  ListNode *left_cur = left->first;
  ListNode *right_cur = right->first;

  // NOTE: in this func, we can NOT use `List_insert_sorted`, it will update the
  // node's `left` and `right` in place, and break the chain of origin list.
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

  if (left_sorted != left) {
    List_destroy(left);
  }
  if (right_sorted != right) {
    List_destroy(right);
  }

  List *ret = merge_sorted_lists(left_sorted, right_sorted, cmp);

  List_destroy(left_sorted);
  List_destroy(right_sorted);

  return ret;
}
