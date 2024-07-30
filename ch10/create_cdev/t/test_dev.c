

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
  int fd = open("/dev/test", O_RDWR);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  if (close(fd) == -1) {
    perror("close");
    return -1;
  }

  printf("===== test success =====\n");

  return 0;
}