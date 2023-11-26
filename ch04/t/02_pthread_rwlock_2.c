
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM  4

volatile int shared = 0;
pthread_rwlock_t rwlock;

void *transaction_write(void *arg)
{
    while (1) {
        if (shared < 20) {
            pthread_rwlock_wrlock(&rwlock);
            shared++;
            printf("w thread %d write shared variable: %d\n", *(int*)arg, shared);
            pthread_rwlock_unlock(&rwlock);
            sleep(1);
        }
    }
    
    return NULL;
}

void *transaction_read(void *arg)
{
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("r thread %d read shared variable: %d\n", *(int*)arg, shared);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    
    return NULL;
}

/**
 * 写线程会紧跟着上一次的写对 shared 变量进行写；
 * 读线程会读取最新的 shared 变量；
 */
int main(int argc, char *argv[])
{
    int ret = -1;
    pthread_t pid_w[THREAD_NUM] = { 0 };
    pthread_t pid_r[THREAD_NUM] = { 0 };
    int index_w[THREAD_NUM] = { 0 };
    int index_r[THREAD_NUM] = { 0 };
    int i;
    
    ret = pthread_rwlock_init(&rwlock, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_rwlock_init failed: %s\n", strerror(ret));
    }
    
    for (i = 0; i < THREAD_NUM; i++) {
        index_w[i] = i;
        ret = pthread_create(&pid_w[i], NULL, transaction_write, &index_w[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: index=%d\n", i);
            continue;
        }
    }
    
    for (i = 0; i < THREAD_NUM; i++) {
        index_r[i] = i;
        ret = pthread_create(&pid_r[i], NULL, transaction_read, &index_r[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: index=%d\n", i);
            continue;
        }
    }
    
    while (1) {
        if (shared == 20) {
            printf("main thread: shared = %d\n", shared);
            for (i = 0; i < THREAD_NUM; i++) {
                pthread_cancel(pid_w[i]);
            }
            for (i = 0; i < THREAD_NUM; i++) {
                pthread_cancel(pid_r[i]);
            }
            break;
        }
    }

cleanup:
    pthread_rwlock_destroy(&rwlock);
    
    return ret;
}