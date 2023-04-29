
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

/**
    <C专家编程>说道:
        被分配的内存总是经过对齐，以适合机器上最大尺寸的原子访问，一个 malloc 请求申请
    的内存大小为方便起见一般被圆整为 2 的乘方。回收的内存可供重新使用，但并没有(方便的)
    办法把它从你的进程移出交还给操作系统。
    ...
    要想获得以后能够返回给系统内核的内存，可以使用 mmap 系统调用来映射 /dev/zero 文件。
    需要返回这种内存时，可以使用 munmap 系统调用。
 */

// /dev/zero + 共享内存 + 大页
// 使用 /dev/zero 必须要指定 MAP_ANONYMOUS 标志位，没搞清楚，暂不考虑

int main(int argc, char *argv[])
{
    int fd = -1;

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char *m;
    size_t s = (8UL * 1024 * 1024);

    m = (char*)mmap(NULL, s, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_HUGETLB | MAP_ANONYMOUS, fd, 0);
    if (m == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    memset(m, 0, s);

    printf("map_hugetlb ok, press ENTER to quit!\n");
    getchar();

    munmap(m, s);

    return 0;
}
