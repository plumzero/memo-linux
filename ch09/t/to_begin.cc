
#include <stdio.h>
#include <unistd.h>

int main()
{
  int i;

  printf("downloading SwordGame from onestraw.net...\n");

  for(i=0; i<=100; i++) {
    printf("finish %d%%...\r", i);
    printf("finish %d\r", i);
    fflush(stdout);
    printf("end %d\r", i);
    fflush(stdout);
    sleep(1);
  }

  printf("\ndone\n");

  return 0;
}