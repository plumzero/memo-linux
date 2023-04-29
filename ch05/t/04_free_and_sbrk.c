
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define _Assert(exp) { if (!(exp)) { exit(EXIT_FAILURE); } }

#define MAX_ALLOCS 1000000

int main()
{
  printf("Initial program break: %p\n", sbrk(0));

  char *ptr[MAX_ALLOCS] = { NULL };

  int i;
  for (i = 0; i < MAX_ALLOCS; i++) {
    ptr[i] = (char *)malloc(1024);
    _Assert(ptr[i] != NULL);
  }

  printf("Current program break: %p\n", sbrk(0));

  for (i = 0; i < 250000; i++) {
    free(ptr[i]);
  }
  printf("free [0, 250000), program break: %p\n", sbrk(0));

  for (i = 250000; i < 500000; i++) {
    free(ptr[i]);
  }
  printf("free [250000, 500000), program break: %p\n", sbrk(0));

  for (i = 750000; i < 1000000; i++) {
    free(ptr[i]);
  }
  printf("free [750000, 1000000), program break: %p\n", sbrk(0));

  for (i = 500000; i < 750000; i++) {
    free(ptr[i]);
  }
  printf("free [500000, 750000), program break: %p\n", sbrk(0));

  return 0;
}