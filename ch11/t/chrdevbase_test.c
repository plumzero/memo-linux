
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static char userdata[] = { "user data!" };

int main(int argc, char * argv[])
{
  int fd, ret;
  char * filename;
  char readbuf[100] = { 0 };
  char writebuf[100] = { 0 };

  if (argc != 3) {
    printf("Error Usage!\n");
    return -1;
  }

  filename = argv[1];

  fd = open(filename, O_RDWR);
  if (fd < 0) {
    perror("open");
    return -1;
  }

  if (atoi(argv[2]) == 1) {
    ret = read(fd, readbuf, 50);
    if (ret < 0) {
      perror("read");
    } else {
      printf("read: %s\n", readbuf);
    }
  }

  if (atoi(argv[2]) == 2) {
    memcpy(writebuf, userdata, sizeof(userdata));
    ret = write(fd, writebuf, strlen(writebuf));
    if (ret < 0) {
      perror("write");
    }
  }

  ret = close(fd);
  if (ret < 0) {
    perror("close");
    return -1;
  }

  return 0;
}