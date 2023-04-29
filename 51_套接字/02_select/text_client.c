#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/tcp.h>

#include "abc.h"

#define BUF_SIZE 500

#define USAGE   \
    "\n  ./* <host> <port or service> <input filename>\n" \
    "\n"

void go_sleep(long ms)
{
	struct timeval timeout;
	timeout.tv_sec = ms / 1000L;
	ms = ms % 1000L;
	timeout.tv_usec = (int)ms * 1000;
	select(0, NULL,  NULL, NULL, &timeout);
}

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
	int fd, i, on, ret = 0;
	uint16_t slen;
    ssize_t nw;
    char sbuf[BUF_SIZE] = { 0 };
	FILE *f;
	unsigned LE = 1;
	unsigned isLittleEndian = *((char*)&LE);

    if (argc < 4) {
		ECHO(ERRO, "%s [ip] [port] [input filename]", argv[0]);
        exit(-1);
    }

	// 设定规则
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;				// IPv4 或 IPv6
    hints.ai_socktype = SOCK_STREAM;			// 流数据报
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;			// TCP协议
	// 获取指定地址上的 addrinfo 结构体
	if ((ret = getaddrinfo(argv[1], argv[2], &hints, &result)) != 0) {
		ECHO(ERRO, "getaddrinfo: %s", gai_strerror(ret));
		goto cleanup;
	}
	// 尝试 result 中的每个地址，直到可以 connect
	ret = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((fd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;
		// 设置 close-on-exec 标志位
		if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
			close(fd);
			continue;
		}
		// 对捕获的中断信号的处理
		while (connect(fd, rp->ai_addr, rp->ai_addrlen) == -1 && errno != EISCONN) {
			if (errno != EINTR) {
				ECHO(ERRO, "connect: %s", strerror(errno));
				close(fd);
				continue;
			}
		}
		// 禁用 Nagle 算法
		on = 1;
		if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&on, sizeof(on)) != 0) {
			ECHO(ERRO, "setsockopt(IPPROTO_TCP, TCP_NODELAY): %s", strerror(errno));
			close(fd);
			continue;
		}
		ret = 0;
        break;
    }
	if (ret) {
		ECHO(ERRO, "connect: %s", strerror(errno));
		goto cleanup;
	}

	if ((f = fopen(argv[3], "r")) == NULL) {
		perror("fopen");
		goto cleanup;
	}
	// 行读取直到 eof
	while((fscanf(f, "%[^\n]", sbuf + 2)) != EOF) {
		fgetc(f);
		slen = (uint16_t)strlen(sbuf + 2);
		if (isLittleEndian) {
			for (i = 0; i < 2; i++)
				sbuf[i] = (unsigned char)(((uint16_t)slen >> ((2 - i - 1) << 3)) & 0xFF);
		} else {
			for (i = 0; i < 2; i++)
				sbuf[i] = (unsigned char)(((uint16_t)slen >> (i << 3)) & 0xFF);
		}
		if ((nw = write(fd, sbuf, slen + 2)) != slen + 2) {
			ECHO(ERRO, "write: %s", strerror(errno));
			goto cleanup;
		}
		ECHO(INFO, "SEND (%2d bytes): %s", slen, sbuf + 2);
		memset(sbuf, 0, BUF_SIZE);
		// 必须加上休眠，否则"粘包"
		go_sleep(10);
	}
	// 读取完毕，再发送一个 eof (0x04) 告知服务端发送完毕
	memset(sbuf, 0, BUF_SIZE);
	slen = 1;
	if (isLittleEndian) {
		for (i = 0; i < 2; i++)
			sbuf[i] = (unsigned char)(((uint16_t)slen >> ((2 - i - 1) << 3)) & 0xFF);
	} else {
		for (i = 0; i < 2; i++)
			sbuf[i] = (unsigned char)(((uint16_t)slen >> (i << 3)) & 0xFF);
	}
	sbuf[2] = 0x04;
	if ((nw = write(fd, sbuf, 3)) != 3) {
		ECHO(ERRO, "write: %s", strerror(errno));
		goto cleanup;
	} else {
		ECHO(INFO, "Transmission over");
	}
	
cleanup:
	// 释放
	if (result)
		freeaddrinfo(result);
	close(fd);
	fclose(f);
	return ret;
}