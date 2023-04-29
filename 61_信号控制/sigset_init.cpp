
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
 
#define USAGE  \
    "Usage: ./cmd <method>\n"  \
    "   method: 1 or 2\n"

/**
 * 1. 信号集初始化的两种方法；
 * 2. pthread_sigmask 类似于 sigprocmask, 不同的是前者用于多线程中；
 * 3. 生产中可以完全拷用此示例，再建议参考方法二；
 */

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        printf(USAGE);
        return -1;
    }
    
    int flag = 0, ret;
    
    if (strcmp(argv[1], "1") == 0) {
        flag = 1;
    } else if (strcmp(argv[1], "2") == 0) {
        flag =2;
    } else {
        printf("invalid parameter\n");
        return -1;
    }
    
    if (flag == 1) {
        printf("信号初始化方法一(参考 nginx): \n"
               "  1. 先将所有信号添加到信号集;\n"
               "  2. 再将一些信号从信号集中删除;\n"
               "  3. 为处理后的信号集设置动作;\n");
        sigset_t            set;
        sigfillset(&set);

        sigdelset(&set, SIGILL);
        sigdelset(&set, SIGFPE);
        sigdelset(&set, SIGSEGV);
        sigdelset(&set, SIGBUS);

        ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
        if (ret) {
            printf("pthread_sigmask error: %s\n", strerror(errno));
        }
    
    } else if (flag == 2) {
        printf("信号初始化方法一(参考 stunnel): \n"
               "  1. 先将信号集设置为空;\n"
               "  2. 再将一些信号添加到信号集中;\n"
               "  3. 为处理后的信号集设置动作;\n");
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGCHLD);
        sigaddset(&set, SIGHUP);
        sigaddset(&set, SIGUSR1);
        sigaddset(&set, SIGUSR2);
        sigaddset(&set, SIGPIPE);
        sigaddset(&set, SIGTERM);
        sigaddset(&set, SIGQUIT);
        sigaddset(&set, SIGINT);
        ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
        if (ret) {
            printf("pthread_sigmask error: %s\n", strerror(errno));
        }
    }
        
    int i;
    for (i = 0; i < 10; i++) {
        printf("%d \n", i);
        sleep(1);
    }
    
    return 0;
}
