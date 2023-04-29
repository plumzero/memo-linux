#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * 用 fork() 实现简单的生产者-消费者功能
 */

int pipe_fd[2];
char r_buf[5];
int r_num;
pid_t result;

int main(int argc, char *argv[])
{
	memset(r_buf, 0, sizeof(r_buf));
	if (pipe(pipe_fd) < 0) {
		printf("pape failed\n");
		exit(-1);
	}
	/** 创建子进程 */
	result = fork();
	if (result < 0) {
		printf("fork failed\n");
		exit(-1);
	}
	
	if (result == 0) {		// 子进程，消费者
		close(pipe_fd[1]);
		while (1) {
			sleep(1);
			if ((r_num = read(pipe_fd[0], r_buf, 5)) == 0)
				break;
			printf("consumer get %s\n", r_buf);
		}
		close(pipe_fd[0]);
		exit(0);
	} else {				// 父进程，生产者
		int i;
		close(pipe_fd[0]);
		for (i = 1; i < 5; i++) {
			sleep(3);
			if (write(pipe_fd[1], "aaaa\0", 5) != -1)
				printf("producer put aaaa\n");
			if (write(pipe_fd[1], "bbbb\0", 5) != -1)
				printf("producer put bbbb\n");
		}
		close(pipe_fd[1]);
		/** 父进程调用 waitpid, 等待子进程结束 */
		waitpid(result, NULL, 0);
		exit(0);
	}
}