
#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif

#include <sched.h>
#include <pthread.h>
#include <unistd.h>     // for sysconf
#include <stdio.h>

/** cpu 个数统计 */
long get_cpu_core_numbers()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

/** 获取当前进程与哪个 cpu 亲和 */
void get_current_process_affinity()
{
    cpu_set_t cs;
    
    CPU_ZERO(&cs);
    
    int ret = sched_getaffinity(0, sizeof(cs), &cs);
    if (ret < 0) {
        /** error occured */
        return;
    }
    
    long cpu_numbers = get_cpu_core_numbers();
    long i;
    for (i = 0; i < cpu_numbers; i++) {
        if (CPU_ISSET((int)i, &cs)) {
            printf("%ld ", i);
        }
    }
    printf("\n");

    return ;
}

/** 设置当前进程与哪个 cpu 亲和 */
int set_current_process_affinity(int core)
{
    cpu_set_t cs;
    
    CPU_ZERO(&cs);
    
    CPU_SET(core, &cs);
    
    int ret = sched_setaffinity(0, sizeof(cs), &cs);
    if (ret < 0) {
        /** error occured */
        return -1;
    }
    
    return 0;
}

/** 获取当前线程与哪个 cpu 亲和 */
void get_current_thread_affinity()
{
    cpu_set_t cs;
    
    CPU_ZERO(&cs);
    
    int ret = pthread_getaffinity_np(pthread_self(), sizeof(cs), &cs);
    if (ret < 0) {
        /** error occured */
        return;
    }
    
    long cpu_numbers = get_cpu_core_numbers();
    long i;
    for (i = 0; i < cpu_numbers; i++) {
        if (CPU_ISSET((int)i, &cs)) {
            printf("%ld ", i);
        }
    }
    printf("\n");

    return ;
}

/** 设置当前线程与哪个 cpu 亲和 */
int set_current_thread_affinity(int core)
{
    cpu_set_t cs;
    
    CPU_ZERO(&cs);
    
    CPU_SET(core, &cs);
    
    int ret = pthread_setaffinity_np(pthread_self(), sizeof(cs), &cs);
    if (ret < 0) {
        /** error occured */
        return -1;
    }
    
    return 0;
}

/** 测试环境: VM+Ubuntu16.04(4 cores) */
int main()
{
    printf("CPU 数量: %ld\n", get_cpu_core_numbers());
    
    printf("当前进程亲和性:");
    get_current_process_affinity();
    printf("当前线程亲和性:");
    get_current_thread_affinity();
    
    printf("\n设置当前线程与 0 号CPU亲和.\n");
    set_current_thread_affinity(0);
    printf("当前进程亲和性:");
    get_current_process_affinity();
    printf("当前线程亲和性:");
    get_current_thread_affinity();
    
    printf("\n设置当前进程与 2 号CPU亲和.\n");
    set_current_thread_affinity(2);
    printf("当前进程亲和性:");
    get_current_process_affinity();
    printf("当前线程亲和性:");
    get_current_thread_affinity();
    
    return 0;
}
