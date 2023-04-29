
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "abc.h"

struct TestHeader
{
	uint16_t 	ver;
	uint16_t	psize;				/** payload size */
	uint8_t 	crc32[4];
	uint32_t	pholder;			/** placeholder */
} __attribute__ ((packed));

#define HBUFSIZE		sizeof(struct TestHeader)
#define DBUFSIZE		1024

#define TOTAL           100000

static const unsigned char crc32[4] = { 0x1f, 0x3d, 0x7e, 0xac };

int main(int argc, char *argv[])
{
	int sfd, cfd;
	struct sockaddr_in ssai;
	struct sockaddr_in csai;
	socklen_t len;
	int on = 1;
	char hbuf[HBUFSIZE];
	char dbuf[DBUFSIZE];
	ssize_t nbytes;
	
	if (argc != 2) {
		ECHO(ERRO, "Usage: %s [port]", argv[0]);
		return -1;
	}
		
	sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sfd == -1) {
		ECHO(ERRO, "socket: %s", strerror(errno));
		return -1;
	}
	
	on = 1;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) != 0) {
		ECHO(ERRO, "setsockopt(SOL_SOCKET, SO_REUSEADDR): %s", strerror(errno));
		close(sfd);
		return -1;
	}
	
	memset(&ssai, 0, sizeof(ssai));
	
	ssai.sin_addr.s_addr = inet_addr("0.0.0.0");
	ssai.sin_port = htons(atoi(argv[1]));
	ssai.sin_family = AF_INET;

	if (bind(sfd, (struct sockaddr*)&ssai, sizeof(struct sockaddr_in)) != 0 ) {
		ECHO(ERRO, "bind: %s", strerror(errno));
		close(sfd);
		return -1;
	}
	
	if (listen(sfd, 5) == -1) {
		ECHO(ERRO, "listen: %s", strerror(errno));
		close(sfd);
		return -1;
	}
	if (fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK) == -1) {
		ECHO(ERRO, "fcntl(F_SETFL, O_NONBLOCK): %s", strerror(errno));
		close(sfd);
		return -1;
	}

	while (1) {
		len = sizeof(struct sockaddr_in);
		cfd = accept(sfd, (struct sockaddr *)&csai, &len);
		if (cfd == -1) {
			if (errno == EINTR || errno == ECONNABORTED || errno == EAGAIN)
				continue;
			else {
				ECHO(ERRO, "accept: %s", strerror(errno));
				close(sfd);
				return -1;
			}
		} else {
			ECHO(INFO, "accept a connection success");
			break;
		}
	}
	
	int count = 0;
	time_t btime = time(NULL);
	
	
	for ( ; ; )
	{	
		/// 窥-读
		nbytes = recv(cfd, hbuf, sizeof(hbuf), MSG_PEEK);
		if (nbytes == 0)
			continue;
		if (nbytes != HBUFSIZE) {
			ECHO(ERRO, "recv(MSG_PEEK): %s", strerror(errno));
			continue;
		}
		/// parse header
		struct TestHeader th;
		memset(&th, 0, sizeof(th));
		memcpy(&th, hbuf, HBUFSIZE);
		
		// DBUG("version=%d, psize=%d, crc32=[%02X %02X %02X %02X], pholder=%d", 
				// th.ver, ntohs(th.psize), th.crc32[0], th.crc32[1], th.crc32[2], th.crc32[3], th.pholder);
		
		int psize = ntohs(th.psize);
		nbytes = recv(cfd, dbuf, HBUFSIZE + psize, 0);
		if (nbytes != (int)HBUFSIZE + psize) {
			ECHO(ERRO, "recv: %s", strerror(errno));
			continue;
		}
		++count;
		// DBUG("(%d)recv %d payload bytes: %s", count, psize, dbuf + HBUFSIZE);
		if (count == TOTAL) {
			time_t etime = time(NULL);
			ECHO(INFO, "cost time %ld seconds", etime - btime);
		}
	}
	
	if (sfd > 0) close(sfd); 
	if (cfd > 0) close(cfd);
	
	return 0;
}

/**
	代替 for ( ; ; ) { ... } 中内容进行 读-读 测试
		/// 读-读
		nbytes = recv(cfd, hbuf, sizeof(hbuf), 0);
		if (nbytes == 0)
			continue;
		if (nbytes != HBUFSIZE) {
			printf("recv failed, %d bytes\n", nbytes);
			continue;
		}
		/// parse header
		TestHeader th;
		memcpy(&th, hbuf, HBUFSIZE);
		
		// DBUG("version=%d, psize=%d, crc32=[%02X %02X %02X %02X], pholder=%d", 
				// th.ver, ntohs(th.psize), th.crc32[0], th.crc32[1], th.crc32[2], th.crc32[3], th.pholder);
		
		int psize = ntohs(th.psize);
		nbytes = recv(cfd, dbuf, psize, 0);
		if (nbytes != psize) {
			printf("recv failed\n");
			continue;
		}
		++count;
		DBUG("(%d)recv %d payload bytes: %s", count, psize, dbuf);
		if (count == TOTAL) {
			time_t etime = time(NULL);
			DBUG("cost time %d seconds\n", etime - btime);
		}
 */

