
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int fd = shm_open("hello", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("shm_open");
    return -1;
  }

  size_t size = 10000;
  if (ftruncate(fd, size) == -1) {
    perror("ftruncate");
    return -1;
  }

  void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  if (close(fd) == -1) {
    perror("close");
    return -1;
  }  

  char buf[128] = "hello world";
  printf("copying %lu bytes\n", strlen(buf));
  memcpy(addr, buf, strlen(buf));

  return 0;
}