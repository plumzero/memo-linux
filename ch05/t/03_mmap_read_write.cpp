
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>

int main(int argc, char* argv[])
{
  if (argc != 2) {
    printf("%s <infile>\n", argv[0]);
    return -1;
  }

  std::string infile = argv[1];
  std::string outfile = infile + ".copy"; 

  // 读
  char* in_addr;
  long fsize;
  {
    int fd = open(infile.c_str(), O_RDONLY);
    if (fd == -1) {
      perror("open");
      return -1;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
      perror("fstat");
      return -1;
    }
    fsize = sb.st_size;
    printf("fsize=%ld\n", fsize);
    in_addr = (char*)mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
    if (in_addr == MAP_FAILED) {
      perror("mmap");
      return -1;
    }
    close(fd);
  }

  // 写
  int fd = open(outfile.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  // 这里可以多分配一些空间，在后面再剪掉
  if (ftruncate(fd, fsize + 100 * 1024 * 1024) == -1) {
    perror("ftruncate");
    return -1;
  }

  char* out_addr = (char*)mmap(NULL, fsize, PROT_WRITE, MAP_SHARED, fd, 0);
  if (out_addr == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  long step = 2048;
  for (long offset = 0; offset < fsize; offset += step) {
    long n = (fsize - offset >= step ? step : fsize - offset);
    memcpy(out_addr + offset, in_addr + offset, n);
  }

  // 剪掉多余空间
  if (ftruncate(fd, fsize) == -1) {
    perror("ftruncate");
    return -1;
  }
  close(fd);

  if (msync(out_addr, fsize, MS_SYNC) == -1) {
    perror("msync");
    return -1;
  }

  if (munmap(out_addr, fsize) == -1) {
    perror("munmap");
    return -1;
  }

  if (munmap(in_addr, fsize) == -1) {
    perror("munmap");
    return -1;
  }  

  printf("===== test success =====\n");

  return 0;
}