
#include <unistd.h>
#include <stdio.h>

/**
 * 获取运行时配置信息
 * long sysconf(int name); 
 */

int main()
{
    long page_size = sysconf(_SC_PAGESIZE);
    printf("page size: %ld\n", page_size);

    return 0;
}