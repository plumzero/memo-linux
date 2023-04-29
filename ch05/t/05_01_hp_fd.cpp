
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// 大页 + 非匿名映射

int main(int argc, char *argv[])
{
    int fd = -1;

    fd = open("/mnt/huge/madolche", O_CREAT | O_RDWR, 0777);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char *m;
    size_t s = (8UL * 1024 * 1024);

    m = (char*)mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (m == MAP_FAILED) {
        perror("mmap");
        m = NULL;
        return 1;
    }

    memset(m, 0, s);

    printf("map_hugetlb ok, press ENTER to quit!\n");
    getchar();

    munmap(m, s);
    
    return 0;
}
