#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 * 父子进程之间，某个进程中静态变量、全局变量或内部变量的改变不会对另一进程
 * 中的对应变量造成影响。
 */

static int glob = 6;

int main()
{
	int var;
	pid_t pid;
	var = 88;

	if ((pid = fork()) < 0) {
		perror("fork error");
	} else if (pid == 0) {
		printf("child process id = %d, parent process id = %d\n", getpid(), getppid());
		glob++;
		var++;
	} else {
		printf("parent process id = %d\n", getpid());
		sleep(2);
	}
	
	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);

	return 0;
}
