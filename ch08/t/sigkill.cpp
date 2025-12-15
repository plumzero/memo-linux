#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * 发送信号示意: 向子进程发送 SIGABRT 信号
 */

int main()
{
	pid_t pid;
	int status;
	
	pid = fork();
	if (pid < 0) {
		printf("fork failed\n");
		return -1;
	}
	
	if (pid == 0) {
		printf("Hi I am child process (%d)\n", (int)getpid());
		sleep(10);
		return 0;
	} else {
		printf("send signal to child process (%d)\n", pid);
		sleep(1);
		kill(pid, SIGABRT);
		wait(&status);
		if (WIFSIGNALED(status)) {
			printf("child process receive signal %d\n", WTERMSIG(status));
		}
	}
	
	return 0;
}
