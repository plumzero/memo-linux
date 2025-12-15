

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * 工作目录是进程启动的目录，比如程序的绝对路径是 /a/b/exe
 * 如果在 /a 下执行 ./b/exe 或 /a/b/exe ，则 /a 就是进程 ./b/exe 或 /a/b/exe 的工作目录；
 * 如果在 /a/b 下执行 ./exe 或 /a/b/exe，则 /a/b 就是进程 ./exe 或 /a/b/exe 的工作目录；
 *
 * 工作目录可能会变化，这种情况通过 chroot + chdir 也没有办法解决。
 * 编程中要注意工作路径与文件路径是否一致。
 */

int main()
{
	char buf[1024] = { 0 };
	printf("当前进程工作路径为: %s\n", getcwd(buf, 1024));

	return 0;
}