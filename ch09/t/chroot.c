
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <curl/curl.h>

/**
 * int chroot(const char *path);
 *  1. 指定 path 所指路径作为当前进程的根目录(/.)；
 *  2. 此根目录会被子进程所继承；
 *  3. 将根目录更换为 path，并不会影响对系统库或其他路径库函数的动态调用；
 *  4. 使用该系统调用的原因:
 *     a. 在经过 chroot 之后，在新根下将访问不到旧系统的根目录结构和文件，这样就增强了系统的安全性；
 *     b. 建立一个与原系统隔离的系统目录结构，方便用户的开发；
 *     c. 换系统的根目录位置，引导 Linux 系统启动以及急救系统等；
 *  5. 但严格来说，此系统调用并不以安全函数作为定位，无论是沙盒一个进程或是限制文件系统函数的调用；
 *  6. 此调用常与 chdir 一起使用；
 *
 * int chdir(const char *path);
 *  1. 改变当前进程的工作目录为 path；
 *  2. 此调用常与 chroot 一起使用；
 * 
 * char *getcwd(char *buf, size_t size);
 *  1. 获取当前工作目录
 *
 * 其他：
 *  1. system 调用要慎用或少用，它可能带来一些不合自己直观预期的结果；
 */
 
/**
 * 编译方式:
 *  gcc -g chroot.c -o chroot -I/usr/local/libcurl/include -L/usr/local/libcurl/lib -lcurl
 */

#define BUF1KSIZE 		1024

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
	
	errno = 0;
	for ( ; ptr = readdir(dir); ) {
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		else
			printf("%s\t", ptr->d_name);	
	}
	printf("\n");
	if (errno != 0) {
		fprintf(stderr, "readdir failed: %s\n", strerror(errno));
		goto cleanup;
	}

	ret = 0;
cleanup:
	if (dir) closedir(dir);
	
	return ret;
}

int get_url_content(char *urlname)
{
	int ret = -1;
	CURL *curl = NULL;
	CURLcode res;
	long response_code;
	
	curl_global_init(CURL_GLOBAL_DEFAULT);
	
	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stderr, "curl_easy_init failed\n");
		goto cleanup;
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, urlname);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
	
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform failed: %s\n", curl_easy_strerror(res));
		goto cleanup;
	}
	
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
	
	printf("GET %s response code: %ld\n", urlname, response_code);
	
	ret = 0;
cleanup:
	if (curl) curl_easy_cleanup(curl);
	curl_global_cleanup();
	
	return ret;
}


int main()
{
	int ret = -1;
	char path[BUF1KSIZE] = { 0 };
	int fd = -1;
	
	if (getcwd(path, BUF1KSIZE) == NULL) {
		fprintf(stderr, "getcwd failed: %s\n", strerror(errno));
		goto cleanup;
	}
	printf("调用 chroot 之前，根目录及根目录下文件: %s\n", path);
	get_dir_files("/");
	
	if (chroot(path) == -1) {
		fprintf(stderr, "chroot failed: %s\n", strerror(errno));
		goto cleanup;
	}
	
	memset(path, 0, BUF1KSIZE);
	if (getcwd(path, BUF1KSIZE) == NULL) {
		fprintf(stderr, "getcwd failed: %s\n", strerror(errno));
		goto cleanup;
	}
	printf("调用 chroot 之后，将当前目录作为根目录。此时新根目录及新根目录下文件: %s\n", path);
	get_dir_files("/");
	
	printf("新根目录的父目录下文件:\n");
	get_dir_files("../");
	
	printf("更换到新根目录后，并不会影响对系统库或其他路径库函数的动态调用:\n");
	get_url_content("10.0.80.121:8090");

	printf("你无法打开原根目录下的一些文件，如 /dev/null :\n");
	fd = open("/dev/null", O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "open(/dev/null) failed: %s\n", strerror(errno));
	}
	
	printf("但环境变量 PWD 的值并不会改变: %s，也就是说，此时的环境变量失效了\n", getenv("PWD"));
	
	printf("使用 chdir 切换当前进程工作目录为新根目录:\n");
	if (chdir(path) == -1) {
		fprintf(stderr, "chdir failed: %s\n", strerror(errno));
		goto cleanup;
	}

	ret = fork();
	if (ret == -1) {
		fprintf(stderr, "fork failed: %s\n", strerror(errno));
		goto cleanup;
	} else if (ret > 0) {
		sleep(1);
	} else if (ret == 0) {
		printf("子进程中继承了父进程对根目录的改变:\n");
		get_dir_files("/");
	}
	
	ret = 0;
cleanup:
	if (fd > 0) close(fd);

	return ret;
}