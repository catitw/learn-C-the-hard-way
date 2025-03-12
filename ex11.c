#include <stdio.h>

int main(int argc, char *argv[]) {

  for (int i = argc - 1; i >= 0; --i) {
    printf("arg %d: %s\n", i, argv[i]);
  }

  // let's make our own array of strings
  char *states[] = {"California", "Oregon", "Washington", "Texas"};

  for (int i = 0; i < (sizeof(states) / sizeof(char *)) && i < argc; ++i) {
    states[i] = argv[i];
  }

  for (int i = 0; i < (sizeof(states) / sizeof(char *)); ++i) {
    printf("%s\n", states[i]);
  }

  return 0;
}
