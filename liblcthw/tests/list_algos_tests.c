#include "lcthw/dbg.h"
#include "minunit.h"
#include <assert.h>
#include <lcthw/list.h>
#include <lcthw/list_algos.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *values[] = {"XXXX", "1234", "abcd", "xjvef", "NDSS"};
#define NUM_VALUES 5

List *create_words() {
  int i = 0;
  List *words = List_create();

  for (i = 0; i < NUM_VALUES; i++) {
    List_push(words, values[i]);
  }

  return words;
}

char *gen_rand_str(size_t length) {
  const char charset[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  char *str = (char *)malloc((length + 1) * sizeof(char));
  check_mem(str);

  for (size_t i = 0; i < length; i++) {
    str[i] = charset[rand() % (sizeof(charset) - 1)];
  }
  str[length] = '\0';
  return str;

error:
  return NULL;
}

char *gen_rand_len_str(size_t min_eq_len, size_t max_eq_len) {
  size_t rand_len = min_eq_len + rand() % (max_eq_len - min_eq_len + 1);

  return gen_rand_str(rand_len);
}

#define LOTS_WORDS_COUNT 51200
#define MIN_RAND_STR_LEN 16
#define MAX_RAND_STR_LEN 64
List *create_lots_words(unsigned long long count) {

  List *l = List_create();

  for (unsigned long long i = 0; i < count; ++i) {
    List_push(l, (void *)gen_rand_len_str(MIN_RAND_STR_LEN, MAX_RAND_STR_LEN));
  }

  return l;
}

int is_sorted(List *words) {
  LIST_FOREACH(words, first, next, cur) {
    if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
      debug("%s %s", (char *)cur->value, (char *)cur->next->value);
      return 0;
    }
  }

  return 1;
}

char *test_bubble_sort() {
  List *words = create_words();

  // should work on a list that needs sorting
  int rc = List_bubble_sort(words, (List_compare)strcmp);
  mu_assert(rc == 0, "Bubble sort failed.");
  mu_assert(is_sorted(words), "Words are not sorted after bubble sort.");

  // should work on an already sorted list
  rc = List_bubble_sort(words, (List_compare)strcmp);
  mu_assert(rc == 0, "Bubble sort of already sorted failed.");
  mu_assert(is_sorted(words), "Words should be sort if already bubble sorted.");

  List_destroy(words);

  // should work on an empty list
  words = List_create(words);
  rc = List_bubble_sort(words, (List_compare)strcmp);
  mu_assert(rc == 0, "Bubble sort failed on empty list.");
  mu_assert(is_sorted(words), "Words should be sorted if empty.");

  List_destroy(words);

  List *lots_words = create_lots_words(10240);
  List_bubble_sort(lots_words, (List_compare)strcmp);
  mu_assert(is_sorted(lots_words), "Words are not sorted after bubble sort.");

  List_clear_destroy(lots_words);

  return NULL;
}

char *test_merge_sort() {
  List *words = create_words();

  // should work on a list that needs sorting
  List *res = List_merge_sort(words, (List_compare)strcmp);
  mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

  List *res2 = List_merge_sort(res, (List_compare)strcmp);
  mu_assert(is_sorted(res), "Should still be sorted after merge sort.");
  List_destroy(res2);
  List_destroy(res);

  List_destroy(words);

  List *lots_words = create_lots_words(10240);
  List *sorted = List_merge_sort(lots_words, (List_compare)strcmp);
  mu_assert(is_sorted(sorted), "Words are not sorted after merge sort.");

  List_clear_destroy(lots_words);
  List_destroy(sorted);

  List *lots_words2 = create_lots_words(10240);
  List *sorted2 = List_merge_sort_bottom_up(lots_words2, (List_compare)strcmp);
  mu_assert(is_sorted(sorted2), "Words are not sorted after merge sort.");

  // the bottom up merge sort do NOT allocate new nodes.
  // List_clear_destroy(sorted2);

  return NULL;
}

// answer
static void ListNode_swap(ListNode *a, ListNode *b) {
  void *temp = a->value;
  a->value = b->value;
  b->value = temp;
}

int List_bubble_sort_ans(List *list, List_compare cmp) {
  int sorted = 1;

  if (List_count(list) <= 1) {
    return 0; // already sorted
  }

  do {
    sorted = 1;
    LIST_FOREACH(list, first, next, cur) {
      if (cur->next) {
        if (cmp(cur->value, cur->next->value) > 0) {
          ListNode_swap(cur, cur->next);
          sorted = 0;
        }
      }
    }
  } while (!sorted);

  return 0;
}

// answer
static List *List_merge_ans(List *left, List *right, List_compare cmp) {
  List *result = List_create();
  void *val = NULL;

  while (List_count(left) > 0 || List_count(right) > 0) {
    if (List_count(left) > 0 && List_count(right) > 0) {
      if (cmp(List_first(left), List_first(right)) <= 0) {
        val = List_shift(left);
      } else {
        val = List_shift(right);
      }

      List_push(result, val);
    } else if (List_count(left) > 0) {
      val = List_shift(left);
      List_push(result, val);
    } else if (List_count(right) > 0) {
      val = List_shift(right);
      List_push(result, val);
    }
  }

  return result;
}

List *List_merge_sort_ans(List *list, List_compare cmp) {
  if (List_count(list) <= 1) {
    return list;
  }

  List *left = List_create();
  List *right = List_create();
  int middle = List_count(list) / 2;

  LIST_FOREACH(list, first, next, cur) {
    if (middle > 0) {
      List_push(left, cur->value);
    } else {
      List_push(right, cur->value);
    }

    middle--;
  }

  List *sort_left = List_merge_sort_ans(left, cmp);
  List *sort_right = List_merge_sort_ans(right, cmp);

  if (sort_left != left)
    List_destroy(left);
  if (sort_right != right)
    List_destroy(right);

  return List_merge_ans(sort_left, sort_right, cmp);
}

char *test_performance() {
  List *words_my_bubble = create_lots_words(LOTS_WORDS_COUNT);
  List *words_my_merge = List_duplicate(words_my_bubble);
  List *words_my_merge_btm = List_duplicate(words_my_bubble);
  List *words_ans_bubble = List_duplicate(words_my_bubble);
  List *words_ans_merge = List_duplicate(words_my_bubble);

  List *ret = NULL;
  clock_t start, end;
  double cpu_time_used = 0;

  start = clock();
  List_bubble_sort(words_my_bubble, (List_compare)strcmp);
  end = clock();
  cpu_time_used = ((double)end - start) / CLOCKS_PER_SEC;
  // 23.770437
  printf("My bubble sort execution time: %.6f seconds\n", cpu_time_used);

  start = clock();
  List_bubble_sort_ans(words_ans_bubble, (List_compare)strcmp);
  end = clock();
  cpu_time_used = ((double)end - start) / CLOCKS_PER_SEC;
  // 41.804159
  printf("Ans bubble sort execution time: %.6f seconds\n", cpu_time_used);

  start = clock();
  ret = List_merge_sort(words_my_merge, (List_compare)strcmp);
  end = clock();
  cpu_time_used = ((double)end - start) / CLOCKS_PER_SEC;
  // 0.088699
  printf("My merge sort execution time: %.6f seconds\n", cpu_time_used);
  List_destroy(ret);

  start = clock();
  ret = List_merge_sort_bottom_up(words_my_merge_btm, (List_compare)strcmp);
  end = clock();
  cpu_time_used = ((double)end - start) / CLOCKS_PER_SEC;
  // 0.000955
  printf("My bottom up merge sort execution time: %.6f seconds\n",
         cpu_time_used);
  List_destroy(ret);

  start = clock();
  ret = List_merge_sort_ans(words_ans_merge, (List_compare)strcmp);
  end = clock();
  cpu_time_used = ((double)end - start) / CLOCKS_PER_SEC;
  // 0.108398
  printf("Ans merge sort execution time: %.6f seconds\n", cpu_time_used);
  List_clear_destroy(ret);

  // TODO: our dup is shallow copy, so we can NOT free the value by following
  // way. List_clear_destroy(words_my_bubble);
  // List_clear_destroy(words_my_merge);
  // List_clear_destroy(words_ans_bubble);
  // List_clear_destroy(words_ans_merge);

  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_bubble_sort);
  mu_run_test(test_merge_sort);
  mu_run_test(test_performance);

  return NULL;
}

RUN_TESTS(all_tests);
