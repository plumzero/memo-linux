
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>	//for sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

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
	int ret = -1;
	int cfd;
	struct sockaddr_in ssai, csai;
	socklen_t len;
	char ip[32] = { 0 };
	char dbuf[DBUFSIZE];
	ssize_t nbytes;
	
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [ip] [port]", argv[0]);
		return -1;
	}
	
	strcpy(ip, argv[1]);
	
	cfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cfd == -1) {
		fprintf(stderr, "socket: %s", strerror(errno));
		return -1;
	}

	memset(&ssai, 0, sizeof(ssai));
	ssai.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, ip, &ssai.sin_addr);
	ssai.sin_family = AF_INET;
	
	if (connect(cfd, (const struct sockaddr*)&ssai, sizeof(struct sockaddr_in)) == -1) {
		fprintf(stderr, "connect: %s", strerror(errno));
		close(cfd);
		return -1;
	}
	
	len = sizeof(struct sockaddr_in);
	ret = getsockname(cfd, (struct sockaddr *)&csai, &len);
	if (ret != 0) {
		fprintf(stderr, "getsockname: %s", strerror(errno));
		close(cfd);
		return -1;
	}
	fprintf(stdout, "clinet(%s:%d)", inet_ntoa(csai.sin_addr), ntohs(csai.sin_port));
	
	int circle = TOTAL;
	int count = 0;
	time_t btime = time(NULL);
	
	for ( ; circle--; )
	{
		memset(dbuf, 0, sizeof(dbuf));
		
		char tbuf[1024];
		int n = snprintf(tbuf, 1024, "========== hello world from client %d ==========", circle);
		if (n < 0 || n >= 1024) {
			fprintf(stderr, "snprintf: %s", strerror(errno));
			continue;
		}
		/** fill header */
		struct TestHeader th;
		memset(&th, 0, sizeof(th));
		th.ver = 1022;
		th.psize = htons(n);
		memcpy(th.crc32, crc32, 4);
		th.pholder = 0;
		memcpy(dbuf, &th, sizeof(th));
		memcpy(dbuf + sizeof(th), tbuf, n);
		
		nbytes = send(cfd, dbuf, n + sizeof(th), 0);
		if (nbytes != n + (int)sizeof(th)) {
			fprintf(stderr, "send: %s", strerror(errno));
			continue;
		}
		++count;
		if (count == TOTAL) {
			time_t etime = time(NULL);
			fprintf(stdout, "cost time %ld seconds", etime - btime);
		}
	}
	
	if (cfd > 0) close(cfd);
	
	return 0;
}
