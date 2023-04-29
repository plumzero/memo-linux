
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>

/**
	DIR *opendir(const char *name);
	int dirfd(DIR *dirp);
	struct dirent *readdir(DIR *dirp);
	
	只有某些系统类型（如 Btrfs, ext2, ext3, ext4）完全支持 d_type
	描述的文件类型。所有应用必须妥善处理 DT_UNKNOWN.
	
	注意相对路径和绝对路径，否则报错
 */

int get_dir_files(const char* dirname)
{
	int ret = -1;
	DIR *dir;
	int fd;
	struct dirent *ptr;
	
	dir = opendir(dirname);
	if (dir == NULL) {
		fprintf(stderr, "opendir(%s) failed: %s\n", dirname, strerror(errno));
		goto cleanup;
	}
	fd = dirfd(dir);
	if (fd == -1) {
		fprintf(stderr, "dirfd failed: %s\n", strerror(errno));
		goto cleanup;
	}
	printf("get directory(%s) stream file descriptor: %d\n", dirname, fd);
	
	// 正常到达流末或错误，readdir 均会返回 NULL。
	// 为了区分两者，在调用 readdir 之前将 errno 设置为 0，有调用之后检测 errno 值判断。
	// 通过 readdir 读取的文件名无序
	errno = 0;
	for ( ; ptr = readdir(dir); ) {
		switch (ptr->d_type) {
			case DT_BLK:
			{
				printf("%-10s is a block device\n", ptr->d_name);
				break;
			}
			case DT_CHR:
			{
				printf("%-10s is a character device\n", ptr->d_name);
				break;
			}
			case DT_DIR:
			{
				if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
					continue;

				printf("%-10s is a directory, now iterator it ...\n", ptr->d_name);
				ret = get_dir_files(ptr->d_name);
				if (ret == -1) {
					fprintf(stderr, "get files at directory %s failed\n", ptr->d_name);
					return -1;
				}
				break;
			}
			case DT_FIFO:
			{
				printf("%-10s is a named pipe (FIFO)\n", ptr->d_name);
				break;
			}
			case DT_LNK:
			{
				printf("%-10s is a symbolic link\n", ptr->d_name);
				break;
			}
			case DT_REG:
			{
				printf("%-10s is a regular file\n", ptr->d_name);
				break;
			}
			case DT_SOCK:
			{
				printf("%-10s is a UNIX domain socket\n", ptr->d_name);
				break;
			}
			case DT_UNKNOWN:
			{
				printf("%-10s type could not be determined at current file system", ptr->d_name);
				break;
			}
			default:
			{	
				fprintf(stderr, "other error\n");
				break;
			}
		}
		
	}
	if (errno != 0) {
		fprintf(stderr, "readdir failed: %s\n", strerror(errno));
		goto cleanup;
	}

	ret = 0;
cleanup:
	if (dir) closedir(dir);
	
	return ret;
}


int main(int argc, char * argv[])
{
	if (argc < 2) {
		printf("./%s <directory>\n", argv[0]);
		return -1;
	}
	
	int ret = -1;
	
	ret = get_dir_files(argv[1]);
	if (ret == -1) {
		fprintf(stderr, "get files at directory %s failed\n", argv[1]);
		return -1;
	}
	
	return 0;
}