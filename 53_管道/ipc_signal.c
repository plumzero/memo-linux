#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/**
 * 复制出一个子进程，父进程向子进程发出信号，子进程收到此信号后结束子进程。
 *
 * 程序执行后，通过 ps -ef | grep ipc_signal 看到有两个 ipc_signal 名字的进程，一父一子，子进程
 * 被暂停，父进程休眠 10s 后向子进程发送结束信号。
 */
int main(int argc, char *argv[])
{
	pid_t p;
	int r;
	p = fork();
	if (p < 0) {
		printf("fork failed\n");
		exit(1);
	}
	if (p == 0) {
		/** 调用 raise 函数，发送 SIGSTOP 使子进程暂停 */
		raise(SIGSTOP);
		exit(0);
	} else {
		printf("child process pid: %d\n", p);
		if (waitpid(p, NULL, WNOHANG) == 0) {  /** 等待子进程结束 */
			sleep(10);
			/** 调用 kill 函数，发送 SIGKILL 信号结束子进程 */
			if (r = kill(p, SIGKILL) == 0) {
				printf("send SIGKILL signal to kill child process %d\n", p);
			} else {
				printf("kill failed\n");
				exit(1);
			}
		}
	}
	
	return 0;
}