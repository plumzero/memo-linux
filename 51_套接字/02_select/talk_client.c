#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "abc.h"

#define BUF_SIZE 500

// 发送数据格式为: 长度 + 字符串
// 从标准输入读取字符串数据，发送给服务器

void signal_handler(int signum)
{
	switch (signum) {
		case SIGPIPE:
		{
			signal(signum, SIG_IGN);
			break;
		}
		case SIGINT:
		case SIGQUIT:
		{
			exit(-1);
		}
		default:
		{
			printf("Could not process signal %d\n", signum);
			break;
		}
	}
}
 
int main(int argc, char *argv[])
{
	signal(SIGPIPE, signal_handler);
	signal(SIGINT,  signal_handler);
	signal(SIGQUIT, signal_handler);
	
	int i, ret = 0;
	int cfd;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	uint16_t slen;
    ssize_t nwrite;
    char buf[BUF_SIZE];
	const unsigned LE = 1;
	unsigned isLittleEndian = *((char*)&LE);

    if (argc < 3) {
        ECHO(WARN, "%s [ip] [port]", argv[0]);
        exit(-1);
    }

	// 设定规则
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    	// IPv4 或 IPv6
    hints.ai_socktype = SOCK_STREAM;	// 流数据报
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;	// TCP协议
	// 获取指定地址上的 addrinfo 结构体
	if ((ret = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
		ECHO(ERRO, "getaddrinfo: %s", gai_strerror(ret));
		goto cleanup;
	}

	// 尝试 result 中的每个地址，直到可以 connect
	ret = -1;
retry:
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((cfd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;

		// 设置 close-on-exec 标志位
		if (fcntl(cfd, F_SETFD, FD_CLOEXEC) == -1) {
			close(cfd);
			continue;
		}
		while (connect(cfd, rp->ai_addr, rp->ai_addrlen) == -1 && errno != EISCONN) {
			sleep(1);
			if (errno != EINTR) {
				ECHO(ERRO, "connect: %s", strerror(errno));
				close(cfd);
				goto retry;			// connec 失败，则此套接字不再可用，close 当前套接并重新创建套接
			}
		}
		ret = 0;
        break;
    }
	if (ret) {
		ECHO(ERRO, "connect failed");
		goto cleanup;
	}

	for(;;) {
		memset(buf, 0, BUF_SIZE);
		// 预留两个字节存储数据长度
		if (fgets(buf + 2, BUF_SIZE - 2, stdin) == NULL) {
			ECHO(ERRO, "fgets: %s", strerror(errno));
			goto cleanup;
		}
		slen = strlen(buf + 2) - 1;
		// 本地字节序转网络字节序
		if (isLittleEndian) {
			for (i = 0; i < 2; i++)
				buf[i] = (unsigned char)(((uint16_t)slen >> ((2 - i - 1) << 3)) & 0xFF);
		} else {
			for (i = 0; i < 2; i++)
				buf[i] = (unsigned char)(((uint16_t)slen >> (i << 3)) & 0xFF);
		}
		if ((nwrite = write(cfd, buf, slen + 2)) != slen + 2) {	// 换行符不发送
			ECHO(ERRO, "write %s", strerror(errno));
			goto cleanup;
		}
		if (strcmp(buf + 2, "q\n") == 0 || strcmp(buf + 2, "Q\n") == 0)
			break;
	}

cleanup:
	// 释放
	if (result)
		freeaddrinfo(result);
	close(cfd);

	return ret;
}