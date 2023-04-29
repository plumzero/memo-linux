
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// system 的简单测试

int main()
{
    int status;

    // 预期状态非 0
    status = system(NULL);
    printf("'NULL': status = %d\n", status);

    // system() 调用会阻塞调用进程
    status = system("sleep 6");
    printf("'sleep 6': status = %d\n", status);

    // 
    status = system("date");
    printf("'date': status = %d\n", status);

    // 连续执行命令时，以最后执行的命令的返回状态为准
    status = system("who; exit 44");
    printf("'who; exit 44': status = %d\n", status);

    // 无效的 shell 命令
    status = system("invalidcommand");
    printf("'invalidcommand': status = %d\n", status);

    printf("====== test over ======\n");

    return 0;
}