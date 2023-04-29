
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void msleep(long ms)
{
	struct timeval timeout;
	timeout.tv_sec = ms / 1000L;
	ms = ms % 1000L;
	timeout.tv_usec = (int)ms * 1000;
	select(0, NULL,  NULL, NULL, &timeout);
}

double tv_diff_secs(struct timeval newer, struct timeval older)
{
	if(newer.tv_sec != older.tv_sec)
		return (double)(newer.tv_sec - older.tv_sec)+
				(double)(newer.tv_usec - older.tv_usec) / 1000000.0;
	else
		return (double)(newer.tv_usec - older.tv_usec) / 1000000.0;
}

/**
 * 通过线程发送信号，模拟条件脉冲实现生产者消费者模型
 *
 * 本程序说明:
 * 1. thread_producer 调用 pthread_cond_wait 后，会释放锁，随后 thread_consumer 获得该锁；
 * 2. thread_producer 在调用 pthread_cond_signal 后会释放锁，thread_consumer 会得到该锁并加锁；
 * 3. thread_consumer 在收到 cond 后，在离开 pthread_cond_wait 之前，会重新获得锁并加锁；
 */

/**
 * 测试环境: 笔记本虚拟机(Lenovo T460 + VMware + Debian8)
 * 测试条件: 5 万次
 *
 * 测试发现:
 * 1. 在 workload 较大时，在运行一段时间后，无论消费者时间相对生产者时间长短，生产消费不
 *    再均匀（生产者先密集生产，消费者再密集消费，不再有规律交替进行）；
 * 2. 这种情况大大降低了处理的效率；
 * 3. 可以通过生产者不断向消费者发送信号的方式进行处理。处理方式如下:
 * 
 * 测试结果:
 * 如果发现生产者消费者处理索引间隔较大时，可以通过生产者向消费者线程不断发送信号催促来提高效率。
 * 其测试结果如下:
 * 1. 不对 idx_p 和 idx_p 作判断时，用时 5100 ms 左右
 * 2. idx_p - idx_c > 10000 时发送信号，用时 2200 ms 左右
 * 3. idx_p - idx_c > 5000  时发送信号，用时 1100 ms 左右
 * 4. idx_p - idx_c > 1000  时发送信号，用时 400  ms 左右
 * 5. idx_p - idx_c > 500   时发送信号，用时 330  ms 左右
 * 6. idx_p - idx_c > 250   时发送信号，用时 250  ms 左右
 * 7. idx_p - idx_c > 100   时发送信号，用时 240  ms 左右
 * 8. idx_p - idx_c > 50    时发送信号，用时 220  ms 左右
 * 9. idx_p - idx_c > 10    时发送信号，用时 180  ms 左右
 */
 
#define WORKLOAD  50000					// 如果队列太大的话，还是用动态内存吧

// 可以将生产者、消费者时间调整后再作测试
#define PRODUCETIME     250				// 生产者生产时间  600  500
#define CONSUMETIME     250				// 消费者消费时间  200  500
 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


const int workload = WORKLOAD;  		// 要求工作量到达 WORKLOAD 停止
int queue[WORKLOAD] = { 0 };			// 生产者消费者队列

int idx_p = 0;
int idx_c = 0;

void *thread_producer(void *arg)
{
	printf("producer start to work...\n");
	// int i = 0;
	int over = 0;	// 消费者是否消费完毕
	int count = 0;
	
	while (idx_p < workload) {
		pthread_mutex_lock(&mutex);
		if (queue[idx_p] == 0) {				// 生产者进行生产
			printf("Producing goods(id = %d) ... \n", idx_p);
			queue[idx_p++] = 1;
		} else {
			printf("produce exception");
		}
		// msleep(PRODUCETIME);				// 模拟生产者生产时间
		pthread_cond_signal(&cond);			// 锁被消费者获得，生产者继续向下执行
		pthread_mutex_unlock(&mutex);		// 生产者等待消费者将锁释放
		while (idx_p - idx_c > 10) {		// 生产者消费者处理索引相差太大时，生产者不断催促消费者
			pthread_cond_signal(&cond);
		}
	}
	
	while (! over) {						// 消费者需要生产者(信号)的催促，不然就怠工了
		count = 0;
		for (idx_p = 0; idx_p < workload; idx_p++) {
			if (queue[idx_p] == 1) {
				count++;
				break;
			}
		}
		if (count) {						// 未消费完全，继续发送信号
			pthread_cond_signal(&cond);
			// msleep(100);					// 为了节省 cpu 资源 ...
		} else {
			over = 1;
		}
	}
}

void *thread_consumer(void *arg)
{
	printf("consumer start to work...\n");
	
	// int i = 0;
	while (idx_c < workload) {
		pthread_mutex_lock(&mutex);
		printf("consumer is waiting for cond\n");
		pthread_cond_wait(&cond, &mutex);	// 消费者等待生产者发送信号
		pthread_mutex_unlock(&mutex);		// 消费者收到信号，得到锁。消费者会立即将锁给生产者，以便其继续生产。
		if (queue[idx_c] == 1) {				// 消费者进行消费
			printf("Consuming goods(id = %d) ... \n", idx_c);
			queue[idx_c++] = 2;
		} else {
			printf("consume exception");
		}
		// msleep(CONSUMETIME);				// 模拟消费者消费时间
	}
}

int main()
{
	pthread_t pid_producer, pid_consumer;
	int i;
	int count = 0;
	struct timeval btv, etv;
	
	gettimeofday(&btv, NULL);
	pthread_create(&pid_consumer, NULL, thread_consumer, (void*)NULL);
	pthread_create(&pid_producer, NULL, thread_producer, (void*)NULL);
	
	pthread_join(pid_producer, NULL);
	pthread_join(pid_consumer, NULL);
	
	gettimeofday(&etv, NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	for (i = 0; i < WORKLOAD; i++) {
		if (queue[i] == 2)
			count++;
	}
	if (count == workload) {
		printf("========== success ==========\n");
		printf("time cost(millisecond): %.f\n", tv_diff_secs(etv, btv) * 1000);
	} else {
		printf("xxxxxxxxxx failure xxxxxxxxxx\n");
	}
	
	return 0;
}
