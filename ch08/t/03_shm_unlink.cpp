
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

/**
 * 执行此函数后，之前创建的 /dev/shm/hello 文件将被删除
 */

int main()
{
  if (shm_unlink("hello") == -1) {
    perror("shm_unlink");
    return -1;
  }

  return 0;
}