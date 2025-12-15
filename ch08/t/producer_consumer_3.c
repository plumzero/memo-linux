#define _GNU_SOURCE		// for CLONE_VM CLONE_SIGHAND CLONE_FS CLONE_FILES
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

/**
 * 用 clone()、信号量、互斥量实现生产者-消费者功能
 *  1. 通过 CLONE_VM 设定子进程和父进程共享内存，即生产者和消费者可以通过共享内存交换数据。
 *  2. 多个进程共享内存需要互斥和同步机制，程序中定义了临界区变量 mutex 和两个信号量 product
 *     和 warehouse，临界区变量用于共享内存操作的互斥，信号量分别实现了生产者和消费者的等待
 *     和同步通信。
 */

pthread_mutex_t mutex;
sem_t product;
sem_t warehouse;

/** 共享内存 */
char buf[10][5];
int idx = 0;

int producer(void *args)
{
	/** 获得生产者编号 */
	int t = *((int*)args);
	int i;
	for (i = 0; i < 10; i++) {
		/** 表现出线程速度差别 */
		sleep(i + 1);
		/** 等待消费者进程中的一个增加信号量 */
		sem_wait(&warehouse);
		pthread_mutex_lock(&mutex);
		
		if (t == 1)
			strcpy(buf[idx], "1111\0");
		if (t == 2)
			strcpy(buf[idx], "2222\0");
		if (t == 3)
			strcpy(buf[idx], "3333\0");
		printf("producer %d produce %s in %d\n", t, buf[idx], idx);
		idx++;
		
		pthread_mutex_unlock(&mutex);
		/** 生产者增加一个信号量，告知消费者可"消费" */
		sem_post(&product);
	}
	printf("producer %d is over!\n", t);
}

int consumer(void *args)
{
	/** 获得消费者编号 */
	int t = *((int*)args);
	int i;
	for (i = 0; i < 10; i++) {
		/** 表现出线程速度差别 */
		sleep(10 - i);
		/** 等待生产者进程中的一个增加信号量 */
		sem_wait(&product);
		pthread_mutex_lock(&mutex);
		
		idx--;
		printf("consumer %d get %s in %d\n", t, buf[idx], idx);
		
		pthread_mutex_unlock(&mutex);
		/** 消费者增加一个信号量，告知生产者可"生产" */
		sem_post(&warehouse);
	}
	printf("consumer %d is over\n", t);
}

int main(int argc, char *argv[])
{
	int i;
	int clone_flag, arg, r;
	char *stack;
	
	pthread_mutex_init(&mutex, NULL);
	sem_init(&product, 0, 0);
	sem_init(&warehouse, 0, 10);	// 指定信号量为 10，表示生产者可生产
	
	clone_flag = CLONE_VM|CLONE_SIGHAND|CLONE_FS|CLONE_FILES;
	for (i = 0; i < 3; i++) {
		/** 创建 6 个轻进程 */
		arg = i + 1;
		stack = (char *)malloc(4096);
		r = clone((void *)producer, &(stack[4095]), clone_flag, (void*)&arg);
		stack = (char *)malloc(4096);
		r = clone((void *)consumer, &(stack[4095]), clone_flag, (void*)&arg);
	}
	exit(1);
}