
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
 
// 大页 + 匿名映射

int main(int argc, char *argv[])
{
    char *m;
    size_t s = (8UL * 1024 * 1024);

    m = (char*)mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (m == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    // printf("MAP_HUGETLB: %0x\n", MAP_HUGETLB);

    memset(m, 0, s);

    printf("map_hugetlb ok, press ENTER to quit!\n");
    getchar();

    munmap(m, s);
    return 0;
}
