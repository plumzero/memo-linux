

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>


/**
 * getcwd    获取当前路径
 * access    访问某个文件
 * unlink    删除某个文件
 */

int main(int argc, char *argv[])
{
	char *path;
	char cwd[PATH_MAX] = { 0 };
	char fname[PATH_MAX] = { 0 };
	
	if (argc < 2) {
		fprintf(stderr, "USAGE: ./%s <file name>\n", argv[0]);
		return -1;
	}
	
	path = getcwd(cwd, PATH_MAX);
	if (!path) {
		fprintf(stderr, "Cannot get current working directory\n");
		return -1;
	}
	snprintf(fname, sizeof(fname), "%s/%s", path, argv[1]);
	
	if (-1 != access(fname, F_OK)) {
		if (0 != unlink(fname)) {
			fprintf(stderr, "Cannot remove %s.\n", fname);
		}
	}
	
	return 0;
}