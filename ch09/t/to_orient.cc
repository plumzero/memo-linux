
#include <stdio.h>
#include <unistd.h>

#include "util.h"

int main()
{
  int i = 0;

  while (1) {
    printf("%d\n", i);
    printf("%02d\n", i);
    int line_cnt = 2;
    if (i % 2) {
      printf("%03d\n", i);
      line_cnt += 1;
    }
    to_up_literately(line_cnt);
    i++;
    sleep(1);
  }

  return 0;
}