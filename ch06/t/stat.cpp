#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
	char etc_name[PATH_MAX] = { 0 };
	struct stat st;
	int n;
	
	if (argc < 2) {
		fprintf(stderr, "USAGE: ./%s <etc file name>\n", argv[0]);
		return -1;
	}
	
	if (0 != stat("/etc", &st)) {
		fprintf(stderr, "etc file is not found! error %i (%s)\n", errno, strerror(errno));
		return -1;
	}
	
	n = snprintf(etc_name, PATH_MAX, "/etc/%s", argv[1]);
	if (n < 0 || n > PATH_MAX) {
		fprintf(stderr, "Could not format etc path\n");
		return -1;
	}
	
	if (0 != stat(etc_name, &st)) {
		fprintf(stderr, "Etc %s not found! error %i (%s)\n", etc_name, errno, strerror(errno));
		return 0;
	}
	fprintf(stdout, "%s's size is %ld bytes\n", argv[0], st.st_size);
	
	return 0;
}