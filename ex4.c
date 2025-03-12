#include <stdio.h>

int main()
{
    int age = 10;
    int height; /* Warning: uninitialized */

    printf("I am %d years old.\n"); /* Warning */
    printf("I am %d inches tall.\n", height);

    return 0;
}
