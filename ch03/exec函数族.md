
进程创建 exec 函数

```c
  #include <unistd.h>

  int execl(const char *path, const char *arg, ...);
  int execlp(const char *file, const char *arg, ...);
  int execle(const char *path, const char *arg, ..., char *const envp[]);
  int execv(const char *path, char *const argv[]);
  int execvp(const char *file, char *const argv[]);
  int execve(const char *path, char *const argv[], char *const envp[]); // 一参：可执行程序完整路径 二参：传递的命令行参数 三参：传递的环境变量
```
6个函数返回值相同，若成功无返回值，若出错返回-1。

- 这六个函数中只有 `execve` 是真正的系统调用函数，其它五个都是库函数，最终都会调用 execve。
- 以 execve 为例，参数 path 代表一个文件路径，即要被执行的程序的路径。argv 是一个数组指针，通过该数组指针向程序传递命令行参数。envp 向程序传递了环境变量。
- 在使用 exec 函数族的时候，一定要加上错误判断语句，因为 exec 很容易执行失败，其中最常见的原因是: 找不到文件路径。
- 当进程调用一种 exec 函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动例程开始执行。因此，一般情况下，在运行 exec 函数后是不会返回原进程的，只有在错误时才会返回 -1。例如下面的程序示例:
```c
	char *str[] = { "ls", NULL };
	for (int i = 0; i < 100; i++)
		if (execve("/bin/ls", str, NULL) < 0) {
			printf("execve error!");
		}
```
虽然程序中要求执行 100 次，但实际只执行 1 次。

关于上面 6 个函数的区别:
- 函数名中含有 l 的，其参数个数不定，参数由所调用程序的命令行参数列表组成，最后一个 NULL 表示结束。
- 函数名中带有 v 的，使用一个字符串数组指针 argv 指向参数列表，该字符串数组和含有字母 l 的函数中的参数列表完全相同，也同样以 NULL 结束。
- 函数名中带有 p 的，可自动在环境变量 path 指定的路径中搜索要执行的程序，一参 file 表示可执行文件名。
- 函数名中带有 e 的，相比其它我一个envp，此字符串数组指针用于指定环境变量。
- 调用 exec 函数族最常见的错误:
  - 找不到文件或路径，此时 errno 被设置为 ENOENT
	- 数组 argv 和 envp 忘记使用 NULL 结束，此时 errno 被设置为 EFAULT
	- 没有对要执行文件的运行权限，此时 errno 被设置为 EACCES

建议优先不使用 execlp 和 execvp 函数。

通常使用 execv 函数就可以了，需要环境变量的话可以使用 execve 。

[使用示例](t/exec.cc)
