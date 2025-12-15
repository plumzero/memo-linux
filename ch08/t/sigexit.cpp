
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

/**
 * 信号退出!
 * 结论:
 *   1. 子线程收到信号退出之后，主线程也会退出；
 *   2. 主线程收到信号退出之后，子线程也会退出；
 */

/** 结论 1 代码 */
void *slave_thread(void *arg)
{
    printf("从线程: 开始运行...\n");
    
    int ret, i;
    sigset_t set;
    // 继承主线程信号集后，将 SIGINT 设置为非阻塞
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    ret = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    if (ret == 0) {
        printf("从线程: 不阻塞 SIGINT 信号!\n");
    }
        
    for (i = 0; i < 10; i++) {
        printf("从线程: 将调用打印 10 次(这是第 %d 次)\n", i + 1);
        sleep(1);
    }
}

int main()
{   
    sigset_t set, old;
    pthread_t pid;
    int ret, i;
        
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    ret = pthread_sigmask(SIG_BLOCK, &set, &old);
    if (ret == 0) {
        printf("主线程: 阻塞所有信号!\n");
    }

    ret = pthread_create(&pid, NULL, slave_thread, NULL);
    if (ret == 0) {
        printf("主线程: 创建从线程成功!\n");
    }

    for (i = 0; i < 10; i++) {
        printf("主线程: 将调用打印 10 次(这是第 %d 次)\n", i + 1);
        sleep(1);
    }

    return 0;
}

/** 结论 2 代码 */
/*
void *slave_thread(void *arg)
{
    printf("从线程: 开始运行...\n");
    
    int i;
    
    for (i = 0; i < 10; i++) {
        printf("从线程: 将调用打印 10 次(这是第 %d 次)\n", i + 1);
        sleep(1);
    }
}

int main()
{   
    sigset_t set, old;
    pthread_t pid;
    int ret, i;
        
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    ret = pthread_sigmask(SIG_BLOCK, &set, &old);
    if (ret == 0) {
        printf("主线程: 阻塞所有信号!\n");
    }

    ret = pthread_create(&pid, NULL, slave_thread, NULL);
    if (ret == 0) {
        printf("主线程: 创建从线程成功!\n");
    }
    // 将主线程设置回默认信号集
    ret = pthread_sigmask(SIG_SETMASK, &old, NULL);
    if (ret == 0) {
        printf("主线程: 设置回系统默认信号集.\n");
    }

    for (i = 0; i < 10; i++) {
        printf("主线程: 将调用打印 10 次(这是第 %d 次)\n", i + 1);
        sleep(1);
    }

    return 0;
}
*/