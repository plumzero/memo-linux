#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>	//for sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>

#include "abc.h"

int main(int argc, char *argv[])
{
	int ret = -1;
	int cfd;
	struct sockaddr_in ssai, csai;
	socklen_t clen;
	char ip[32] = { 0 };
	char buf[1024] = {0};
	ssize_t nbytes;
	int port;

	(void) nbytes;

	if (argc != 3) {
		ECHO(ERRO, "Usage: %s [ip] [port]", argv[0]);
		return -1;
	}
	
	strcpy(ip, argv[1]);
	port = atoi(argv[2]);

	cfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cfd == -1) {
		ECHO(ERRO, "socket: %s", strerror(errno));
		goto cleanup;
	}

	memset(&ssai, 0, sizeof(ssai));
	ssai.sin_port = htons(port);
	inet_pton(AF_INET, ip, &ssai.sin_addr);
	ssai.sin_family = AF_INET;
	if (connect(cfd, (const struct sockaddr*)&ssai, sizeof(struct sockaddr_in)) == -1) {
		ECHO(ERRO, "connect: %s", strerror(errno));
		goto cleanup;
	}
	
	clen = sizeof(struct sockaddr_in);
	ret = getsockname(cfd, (struct sockaddr *)&csai, &clen);
	if (ret != 0) {
		ECHO(ERRO, "getsockname: %s", strerror(errno));
		goto cleanup;
	}
	ECHO(INFO, "clinet(%s:%d)", inet_ntoa(csai.sin_addr), ntohs(csai.sin_port));
	
	for ( ; ; ) {
		memset(buf, 0, sizeof(buf));
		fscanf(stdin, "%s", buf);
		nbytes = send(cfd, buf, sizeof(buf), 0);
		if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
			break;
		
		buf[0] = '\0';
		nbytes = recv(cfd, buf, sizeof(buf), 0);
		if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
			break;
		ECHO(INFO, "from server(%s:%d): %s\n", inet_ntoa(ssai.sin_addr), ntohs(ssai.sin_port), buf);
	}
	
	ret = 0;
cleanup:
	if (cfd > 0) close(cfd);
	
	return ret;
}
