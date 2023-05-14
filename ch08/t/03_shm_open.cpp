
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * 创建一个 10000 字节的共享内存对象，然后在 /dev/shm 中使用 ls 命令显示出这个对象
 * 
 * 执行后查看
 * $ ls -al /dev/shm/hello
 * -rwxrwxr-x 1 ???? ???? 10000 5月  14 09:37 /dev/shm/hello
 */

int main()
{
  int flags = O_CREAT | O_RDWR;
  mode_t perms = S_IRUSR | S_IWUSR;

  int fd = shm_open("hello", flags, perms);
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

  return 0;
}