
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

/**
 * char *realpath(const char *path, char *resolved_path);
 * 作用: 获取 path 路径的绝对路径
 * 说明: 如果 resolved_path 不为空，则绝对路径通过 resolved_path 指定的缓存保存；
 *       如果 resolved_path 为空，则通过动态内存由函数返回；
 *
 * realpath 缓冲区溢出问题
 *   如果 resolved_path 不为空时，因为函数没有对缓存长度进行限制，当获取的绝对路
 *   径长度超出缓存长度或进行恶意传参时，会造成缓冲区溢出问题；
 * 解决办法
 *   将第二个参数，也即 resolved_path 传入参数置空，结果只通过函数返回，从而避免
 *   缓冲区溢出问题。
 */

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("%s <file name>\n", argv[0]);
		return -1;
	}
	
	char *real_path = NULL;
	
	real_path = realpath(argv[1], NULL);
	if (real_path == NULL) {
		printf("realpath failed: %s\n", strerror(errno));
		return -1;
	}
	
	printf("real path: %s\n", real_path);
	
	free(real_path);

	return 0;
}