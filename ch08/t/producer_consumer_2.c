#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * 用 fork() 实现复杂的生产者-消费者功能
 *   创建 6 个子进程，其中 3 个子进程作为生产者，3 个子进程作为消费者
 */

int pipe_fd[2];
char r_buf[5];		// 读缓冲
char w_buf[5];      // 写缓冲
pid_t pid1, pid2, pid3, pid4, pid5, pid6;

/** 生产者函数 */
int producer(int arg)
{
	int i;
	/** 提示生产者已开始工作 */
	printf("producer %d is starting!\n", arg);
	close(pipe_fd[0]);		// 关闭读端的一个引用计数
	for (i = 0; i < 10; i++) {
		sleep(2);
		if (arg == 1)   // 生产者1
			strcpy(w_buf, "1111\0");
		if (arg == 2)   // 生产者2
			strcpy(w_buf, "2222\0");
		if (arg == 3)   // 生产者4
			strcpy(w_buf, "3333\0");
		/** 向管道中写数据 */
		if (write(pipe_fd[1], w_buf, 5) == -1)
			printf("write failed\n");
	}
	close(pipe_fd[1]);		// 关闭写端的一个引用计数
	printf("producer %d is over!\n", arg);
	exit(arg);
}

/** 消费者函数 */
int consumer(int arg)
{
	/** 提示消费者已开始工作 */
	printf("consumer %d is starting!\n", arg);
	close(pipe_fd[1]);		// 关闭写端的一个引用计数
	if (arg == 1)   // 消费者1
		strcpy(w_buf, "4444\0");
	if (arg == 2)   // 消费者2
		strcpy(w_buf, "5555\0");
	if (arg == 3)   // 消费者4
		strcpy(w_buf, "6666\0");
	while (1) {
		sleep(1);
		/** 消费者从管道中读取数据 */
		if (read(pipe_fd[0], r_buf, 5) == 0)
			break;
		printf("consumer %d get %s, while the w_buf is %s\n", arg, r_buf, w_buf);
	}
	close(pipe_fd[0]);
	printf("consumer %d is over!\n", arg);
	exit(arg);
}

int main(int argc, char *argv[])
{
	int i, pid, status;
	if (pipe(pipe_fd) < 0) {
		printf("pipe failed\n");
		exit(-1);
	}
	
	pid1 = fork();		// 子进程1；生产者1
	if (pid1 == 0) {
		producer(1);
	}
	pid2 = fork();		// 子进程2；生产者2
	if (pid2 == 0) {
		producer(2);
	}
	pid3 = fork();		// 子进程3；生产者3
	if (pid3 == 0) {
		producer(3);
	}
	pid4 = fork();		// 子进程4；消费者1
	if (pid4 == 0) {
		consumer(1);
	}
	pid5 = fork();		// 子进程1；消费者2
	if (pid5 == 0) {
		consumer(2);
	}
	pid6 = fork();		// 子进程1；消费者3
	if (pid6 == 0) {
		consumer(3);
	}
	/** 下面两句如不加上，将会导致读者或者写者永远等待（阻塞在 consumer 函数中的 read 调用中） */
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	/** 等待 6 个子进程结束 */
	for (i = 0; i < 6; i++)
		pid = wait(&status);
	exit(0);
}