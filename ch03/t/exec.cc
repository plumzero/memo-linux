
#include <unistd.h>
#include <stdio.h>

// exec family 函数的使用和对比
// 可以看出: 除 execlp 和 execvp 函数外，其他函数路径名均为全路径

int main(void)
{
	char *envp[] = {"PATH=/home", "USER=lb", "STATUS=testing", NULL};
	char *argv_execv[] = {"echo", "excuted by execv", NULL};
	char *argv_execvp[] = {"echo", "excuted by execvp", NULL};
	char *argv_execve[] = {"env", NULL};
	int ret = 0;
	
	if (fork() == 0) {
		int ret = execl("/bin/echo", "echo", "executed by execl", NULL);
		if (ret == -1)
			perror("execl failed\n");
	}

	if (fork() == 0) {
		int ret = execlp("echo", "echo", "executed by execlp", NULL);
		if (ret == -1)
			perror("execlp failed\n");
	}

	if (fork() == 0) {
		char *envp[] = {"PATH=/home", "USER=lb", "STATUS=testing", NULL};
		int ret = execle("/usr/bin/env", "env", NULL, envp);
		if (ret == -1)
			perror("execle failed\n");
	}

	if (fork() == 0) {
		char *argv[] = {"echo", "excuted by execv", NULL};
		int ret = execv("/bin/echo", argv);
		if (ret == -1)
			perror("execv failed\n");
	}

	if (fork() == 0) {
		char *argv[] = {"echo", "excuted by execvp", NULL};
		int ret = execvp("echo", argv);
		if (ret == -1)
			perror("execvp failed\n");
	}

	if (fork() == 0) {
		char *argv[] = {"env", NULL};
		char *envp[] = {"PATH=/home", "USER=lb", "STATUS=testing", NULL};
		int ret = execve("/usr/bin/env", argv, envp);
		if (ret == -1)
			perror("execve failed\n");
	}

	return 0;
}
