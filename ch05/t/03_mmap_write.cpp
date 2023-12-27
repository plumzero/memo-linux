
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
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

  FILE* fin = fopen(infile.c_str(), "rb");
  if (fin == NULL) {
    perror("fopen");
    return -1;
  }

  fseek(fin, 0, SEEK_END);
  long fsize = ftell(fin);
  fseek(fin, 0, SEEK_SET);

  printf("fsize=%ld\n", fsize);

  // 文件映射
  int fd = open(outfile.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  // 必须
  if (ftruncate(fd, fsize) == -1) {
    perror("ftruncate");
    return -1;
  }

  char* addr = (char*)mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  if (close(fd) == -1) {
    perror("close");
    return -1;
  }

  unsigned char buf[1024] = {0};
  long step = sizeof(buf);
  for (long offset = 0; offset < fsize; offset += step) {
    long n = (fsize - offset >= step ? step : fsize - offset);
    if (fread(buf, 1, n, fin) != (size_t)n) {
      fclose(fin);
      return -1;
    }
    memcpy(addr + offset, buf, n);
  }

  // 可有可无
  if (msync(addr, fsize, MS_SYNC) == -1) {
    perror("msync");
    return -1;
  }

  if (munmap(addr, fsize) == -1) {
    perror("munmap");
    return -1;
  }

  fclose(fin);

  printf("===== test success =====\n");

  return 0;
}