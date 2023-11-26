
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
/**
 * 通过线程发送信号，模拟条件脉冲
 *
 * 本程序说明:
 * 1. 线程 b 调用 pthread_cond_wait 后，会释放锁，随后线程 a 获得该锁；
 * 2. 线程 a 在调用 pthread_cond_signal 后会释放锁，线程 b 会得到该锁并加锁；
 * 3. 线程 b 在收到 cond 后，在离开 pthread_cond_wait 之前，会重新获得锁并加锁；
 *
 */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int idx = 1;

void *thread_a(void *arg)
{
	printf("thread a: enter with idx = %d\n", idx);
	for (idx = 1; idx <= 15; idx++) {
		int flag = 0;
		pthread_mutex_lock(&mutex);
		if (idx % 5 == 0) {
			printf("thread a: send a cond\n");
			pthread_cond_signal(&cond);
			flag = 1;
		} else {
			printf("thread a: is running\n");
		}

		if (flag)		// 只给线程 b 2 秒的处理时间，如果超时则 a 会继续向下执行
		{
			printf("thread a: give thread b 2 seconds deal\n");
			sleep(2);
		}
		
		pthread_mutex_unlock(&mutex);		// 如果线程 b 仍占有锁，则线程 a 只能在等待线程 b 释放后，自己才能释放锁
		printf("thread a: sleep for 1 seconds\n");
		sleep(1);
	}
}

void *thread_b(void *arg)
{
	printf("thread b: enter with idx = %d\n", idx);
	while(idx <= 15)
	{
		pthread_mutex_lock(&mutex);
		if (idx % 5 != 0) {
			printf("thread b: wait for cond\n");
			pthread_cond_wait(&cond, &mutex);
			printf("thread b: get the cond\n");
		}
		printf("thread b: sleep for 5 seconds\n");
		sleep(5);		// 让线程 a 等着，锁在这里	
		pthread_mutex_unlock(&mutex);
		printf("thread b: work for 10 seconds\n");
		sleep(10);
	}
}

int main()
{
	pthread_t pid_a;
	pthread_t pid_b;
	
	pthread_create(&pid_b, NULL, thread_b, (void*)NULL);
	sleep(1);
	pthread_create(&pid_a, NULL, thread_a, (void*)NULL);
	
	pthread_join(pid_a, NULL);
	pthread_join(pid_b, NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	return 0;
}
