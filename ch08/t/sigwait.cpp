
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

/** sigwait 的用法 */

static void* signal_thread(void *arg)
{
    sigset_t *set = (sigset_t*)arg;
    int ret, sig;

    for (;;) {
        ret = sigwait(set, &sig);
        if (ret != 0) {
            printf("sigwait: %s\n", strerror(errno));
            return nullptr;
        }
        printf("signal handling thread got signal %d\n", sig);
        /** 将 sig 信号设置为非阻塞 */
        ret = sigprocmask(SIG_UNBLOCK, set, NULL);
        if (ret != 0) {
            printf("sigprocmask: %s\n", strerror(errno));
            return nullptr;
        }
        
        printf("now try again\n");
    }
    
    return nullptr;
}

int main(int argc, char *argv[])
{
    pthread_t pid;
    sigset_t set;
    int ret;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    ret = sigprocmask(SIG_BLOCK, &set, NULL);
    if (ret != 0) {
       printf("sigprocmask: %s\n", strerror(errno));
       return -1;
    }

    ret = pthread_create(&pid, NULL, &signal_thread, (void *)&set);
    if (ret != 0) {
        printf("pthread_create: %s\n", strerror(errno));
        return -1;
    }

    for (;;) { sleep(2); }

    return 0;
}