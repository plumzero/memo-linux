#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

/**
 * 用 pthread_create() 实现生产者-消费者的功能
 */

sem_t product;
sem_t warehouse;
/** mutex 是用来实现互斥的信号量，不再是互斥锁了 */
sem_t mutex;

/** 共享内存 */
char buf[10][5];
int idx = 0;
pthread_t p1, p2, p3, p4, p5, p6;

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
		/** 实现加锁功能 */
		sem_wait(&mutex);
		
		if (t == 1)
			strcpy(buf[idx], "1111\0");
		if (t == 2)
			strcpy(buf[idx], "2222\0");
		if (t == 3)
			strcpy(buf[idx], "3333\0");
		printf("producer %d produce %s in %d\n", t, buf[idx], idx);
		idx++;
		
		sem_post(&mutex);
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
		sem_wait(&mutex);
		
		idx--;
		printf("consumer %d get %s in %d\n", t, buf[idx], idx);
		
		sem_post(&mutex);
		/** 消费者增加一个信号量，告知生产者可"生产" */
		sem_post(&warehouse);
	}
	printf("consumer %d is over\n", t);
}

int main(int argc, char *argv[])
{
	int i;
	int arg;
	
	sem_init(&product, 0, 0);
	sem_init(&warehouse, 0, 10);	// 指定信号量为 10，表示生产者可生产
	sem_init(&mutex, 0, 1);			// 信号量 mutex 的初始值必须为 1
	
	/** 创建 6 个线程 */
	arg = 1;
	pthread_create(&p1, NULL, (void*)producer, (void*)&arg);
	pthread_create(&p2, NULL, (void*)consumer, (void*)&arg);

	arg = 2;
	pthread_create(&p3, NULL, (void*)producer, (void*)&arg);
	pthread_create(&p4, NULL, (void*)consumer, (void*)&arg);

	arg = 3;
	pthread_create(&p5, NULL, (void*)producer, (void*)&arg);
	pthread_create(&p6, NULL, (void*)consumer, (void*)&arg);
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(p4, NULL);
	pthread_join(p5, NULL);
	pthread_join(p6, NULL);
	
	exit(1);
}