
#include <stdio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>

/**
 * SIGHUP 信号捕获演示
  实验场景：
    在 SecureCRT （其他控制终端亦可）上打开一个控制终端，执行此可执行文件，随后关闭该窗口，
    可以看到在 /tmp 目录下生成了 sighup.txt 文件。
  说明: 
    该进程依赖于此当前控制终端，一旦断开了与终端的连接（比如关闭了终端窗口），控制进程
    将会收到 SIGHUP 信号。
    即便以 ./sighup & 的方式运行程序，关闭控制终端时，仍然会发送同样的信号。
 */

void signal_handler(int signum)
{
    switch (signum) {
        case SIGHUP:
        {
            printf("\nSignal %d received, preparing to hangle the process...\n", signum);
            signal(signum, SIG_DFL);
            FILE* fp = fopen("/tmp/sighup.txt", "wb");
            if (fp) fclose(fp);
            break;
        }
        default:
        {
            printf("Could not process this signal\n");
            break;
        }
    }
}
 
int main()
{
    if (SIG_ERR == signal(SIGHUP, signal_handler)) {
        return -1;
    }
    
    int i;
    for (i = 0; i < 1000; i++) {
        printf("%d \n", i);
        sleep(1);
    }
    
    return 0;
}