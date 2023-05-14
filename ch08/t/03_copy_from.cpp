
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


int main()
{
  int fd = shm_open("hello", O_RDONLY, 0);
  if (fd == -1) {
    perror("shm_open");
    return -1;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    return -1;
  }

  long size = sb.st_size;
  // printf("size: %ld", size);

  void* addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  if (close(fd) == -1) {
    perror("close");
    return -1;
  }  

  (void) write(STDOUT_FILENO, addr, size);
  printf("\n");

  return 0;
}