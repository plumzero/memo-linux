
#include <stdio.h>
#include <unistd.h>
#include "util.h"

int main()
{
  printf(RED "=====%d\n" RESET, 1000);
  printf(GREEN "=====%d\n" RESET, 1000);
  printf(YELLOW "=====%d\n" RESET, 1000);
  printf(BLUE "=====%d\n" RESET, 1000);
  printf(PURPLE "=====%d\n" RESET, 1000);

  return 0;  
}