#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define SIZE 1024

/**
 * 1. 父进程调用 pipe 开辟管道，得到两个文件描述符指向管道的两端
 * 2. 父进程调用 fork 创建子进程，那么子进程也有两个文件描述符指向同一管道
 * 3. 父进程关闭管道读端，子进程关闭管道写端。父进程写，子进程读，实现进程间通信。
 */
int main(int argc, char *argv[])
{
	int n;
	int fd[2];
	pid_t p;
	char str[SIZE];
	if (pipe(fd) < 0) {
		printf("pipe failed\n");
		exit(1);
	}	
	p = fork();
	if (p < 0) {
		printf("fork failed\n");
		exit(1);
	}
	if (p > 0) {
		printf("parent process write:\n");
		close(fd[0]);    /** 关闭读端 */
		write(fd[1], "hello world", strlen("hello world"));
	} else {
		printf("child process read:\n");
		close(fd[1]);    /** 关闭写端 */
		n = read(fd[0], str, SIZE);
		printf("%s\n", str);
	}
	
	return 0;
}