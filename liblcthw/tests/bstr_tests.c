#include "minunit.h"
#include <lcthw/bstrlib.h>
#include <string.h>

#define B_FROM_STR(name, str)                                                  \
  bstring name = bfromcstr(str);                                               \
  mu_assert(name != NULL, "bfromcstr failed");

char *test_bfromcstr() {
  B_FROM_STR(b, "Hello, world!");
  B_FROM_STR(b2, "Hello, world!");

  mu_assert(biseq(b, b2), "bfromcstr content mismatch");

  bdestroy(b);
  bdestroy(b2);

  return NULL;
}

char *test_blk2bstr() {
  const char *data = "Hello, world!";
  bstring b = blk2bstr(data, 5);
  mu_assert(b != NULL, "blk2bstr failed");

  B_FROM_STR(b2, "Hello");
  mu_assert(biseq(b, b2), "blk2bstr content mismatch");

  bdestroy(b);
  bdestroy(b2);
  return NULL;
}

char *test_bstrcpy() {
  B_FROM_STR(b1, "Copy this!");
  bstring b2 = bstrcpy(b1);
  mu_assert(biseq(b1, b2), "bstrcpy failed");
  bdestroy(b1);
  bdestroy(b2);
  return NULL;
}

char *test_bassign() {
  B_FROM_STR(b1, "Original");
  B_FROM_STR(b2, "New content");
  int result = bassign(b1, b2);
  mu_assert(result == BSTR_OK, "bassign failed");
  mu_assert(biseq(b1, b2), "bassign content mismatch");
  bdestroy(b1);
  bdestroy(b2);
  return NULL;
}

char *test_bassigncstr() {
  B_FROM_STR(b, "Old content");
  int result = bassigncstr(b, "New content");
  mu_assert(result == BSTR_OK, "bassigncstr failed");

  B_FROM_STR(b2, "New content");
  mu_assert(biseq(b, b2), "bassigncstr content mismatch");

  bdestroy(b);
  bdestroy(b2);

  return NULL;
}

char *test_bassignblk() {
  B_FROM_STR(b, "Old content");
  int result = bassignblk(b, "Block content", 5);
  mu_assert(result == BSTR_OK, "bassignblk failed");

  B_FROM_STR(b2, "Block");
  mu_assert(biseq(b, b2), "bassignblk content mismatch");

  bdestroy(b);
  bdestroy(b2);

  return NULL;
}

char *test_bdestroy() {
  B_FROM_STR(b, "To be destroyed");
  bdestroy(b);
  // No direct way to test if memory was freed, but Valgrind will catch leaks.
  return NULL;
}

char *test_bconcat() {
  B_FROM_STR(b1, "Hello");
  B_FROM_STR(b2, ", world!");
  int result = bconcat(b1, b2);
  mu_assert(result == BSTR_OK, "bconcat failed");

  B_FROM_STR(b3, "Hello, world!");
  mu_assert(biseq(b1, b3), "bconcat content mismatch");

  bdestroy(b1);
  bdestroy(b2);
  bdestroy(b3);
  return NULL;
}

char *test_bstricmp() {
  B_FROM_STR(b1, "case");
  B_FROM_STR(b2, "CASE");
  int result = bstricmp(b1, b2);
  mu_assert(result == 0, "bstricmp failed");
  bdestroy(b1);
  bdestroy(b2);
  return NULL;
}

char *test_biseq() {
  B_FROM_STR(b1, "Equal");
  B_FROM_STR(b2, "Equal");
  mu_assert(biseq(b1, b2), "biseq failed");
  bdestroy(b1);
  bdestroy(b2);
  return NULL;
}

char *test_binstr() {
  B_FROM_STR(b1, "Find the substring");
  B_FROM_STR(b2, "substring");
  int index = binstr(b1, 0, b2);
  mu_assert(index == 9, "binstr failed");
  bdestroy(b1);
  bdestroy(b2);
  return NULL;
}

char *test_bfindreplace() {
  B_FROM_STR(b, "Replace the word");
  B_FROM_STR(find, "word");
  B_FROM_STR(replace, "phrase");
  int result = bfindreplace(b, find, replace, 0);
  mu_assert(result == BSTR_OK, "bfindreplace failed");

  B_FROM_STR(b2, "Replace the phrase");
  mu_assert(biseq(b, b2), "bfindreplace content mismatch");

  bdestroy(b);
  bdestroy(b2);
  bdestroy(find);
  bdestroy(replace);
  return NULL;
}

char *test_bsplit() {
  B_FROM_STR(b, "Split, this, string");
  struct bstrList *list = bsplit(b, ',');
  mu_assert(list != NULL, "bsplit failed");

  mu_assert(list->qty == 3, "bsplit returned incorrect number of elements");

  B_FROM_STR(b2, "Split");
  mu_assert(biseq(list->entry[0], b2), "bsplit content mismatch");

  B_FROM_STR(b3, " this");
  mu_assert(biseq(list->entry[1], b3), "bsplit content mismatch");

  B_FROM_STR(b4, " string");
  mu_assert(biseq(list->entry[2], b4), "bsplit content mismatch");

  bstrListDestroy(list);
  bdestroy(b);
  bdestroy(b2);
  bdestroy(b3);
  bdestroy(b4);
  return NULL;
}

char *test_bformat() {
  bstring b = bformat("Formatted %d %s", 42, "answer");
  mu_assert(b != NULL, "bformat failed");

  B_FROM_STR(b2, "Formatted 42 answer");
  mu_assert(biseq(b, b2), "bformat content mismatch");

  bdestroy(b);
  bdestroy(b2);
  return NULL;
}

char *test_blength() {
  B_FROM_STR(b, "Length test");
  mu_assert(blength(b) == 11, "blength failed");
  bdestroy(b);
  return NULL;
}

char *test_bdata() {
  B_FROM_STR(b, "Data test");

  char *data = bdata(b);
  mu_assert(data != NULL, "bdata failed");
  mu_assert(strcmp(data, "Data test") == 0, "bdata failed");
  bdestroy(b);

  return NULL;
}

char *test_bchar() {
  B_FROM_STR(b, "Character test");
  mu_assert(bchar(b, 0) == 'C', "bchar failed");
  mu_assert(bchar(b, 10) == 't', "bchar failed");
  bdestroy(b);
  return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_bfromcstr);
  mu_run_test(test_blk2bstr);
  mu_run_test(test_bstrcpy);
  mu_run_test(test_bassign);
  mu_run_test(test_bassigncstr);
  mu_run_test(test_bassignblk);
  mu_run_test(test_bdestroy);
  mu_run_test(test_bconcat);
  mu_run_test(test_bstricmp);
  mu_run_test(test_biseq);
  mu_run_test(test_binstr);
  mu_run_test(test_bfindreplace);
  mu_run_test(test_bsplit);
  mu_run_test(test_bformat);
  mu_run_test(test_blength);
  mu_run_test(test_bdata);
  mu_run_test(test_bchar);

  return NULL;
}

RUN_TESTS(all_tests);
