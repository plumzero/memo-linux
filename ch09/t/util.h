
#pragma once

#include <stdio.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define PURPLE  "\033[35m"

void to_up(int n) {
  printf("\033[%uA", n);
  printf("\033[K");
}

void to_up_literately(int n) {
  if (n <= 0) return ;
  for (int i = 0; i < n; i++) {
    printf("\033[1A");
    printf("\033[K");
  }
}
