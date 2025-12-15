#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "abc.h"

int main(int argc, char *argv[])
{
	int ret = -1;
	int sfd, cfd;
	struct sockaddr_in ssai;
	struct sockaddr_in csai;
	socklen_t clen;
	int on = 1;
	char buf[1024] = { 0 };
	ssize_t nbytes;
	int port;
	const char * ip = "0.0.0.0";
	
    (void) nbytes;

	if (argc != 2) {
		ECHO(ERRO, "Usage: %s [port]", argv[0]);
		return -1;
	}

	port = atoi(argv[1]);

	sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sfd == -1) {
		ECHO(ERRO, "socket: %s", strerror(errno));
		goto cleanup;
	}
	
	on = 1;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) != 0) {
		ECHO(ERRO, "setsockopt(IPPROTO_TCP, SO_REUSEADDR): %s", strerror(errno));
		goto cleanup;
	}
	
	memset(&ssai, 0, sizeof(ssai));
	ssai.sin_port = htons(port);
	inet_pton(AF_INET, ip, &ssai.sin_addr);
	ssai.sin_family = AF_INET;
	if (bind(sfd, (struct sockaddr*)&ssai, sizeof(struct sockaddr_in)) != 0 ) {
		ECHO(ERRO, "bind: %s", strerror(errno));
		goto cleanup;
	}
	
	if (listen(sfd, 5) == -1) {
		ECHO(ERRO, "listen: %s", strerror(errno));
		goto cleanup;
	}

	// // 通常，设置监听套接为非阻塞操作在 bind listen 完成之后进行
	// // 非复用情况下不需要设置为非阻塞
	// if (fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK) == -1) {
	// 	ECHO(ERRO, "fcntl(F_SETFL, O_NONBLOCK): %s", strerror(errno));
	// 	goto cleanup;
	// }
	
	clen = sizeof(struct sockaddr_in);
	cfd = accept(sfd, (struct sockaddr *)&csai, &clen);
	if (cfd == -1) {
		ECHO(ERRO, "accept: %s", strerror(errno));
		goto cleanup;
	}
	
	for ( ; ; ) {
		memset(buf, 0, sizeof(buf));
		nbytes = recv(cfd, buf, sizeof(buf), 0);
		if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
			break;
		ECHO(INFO, "from client(%s:%d): %s", inet_ntoa(csai.sin_addr), ntohs(csai.sin_port), buf);
		
		buf[0] = '\0';
		fscanf(stdin, "%s", buf);
		nbytes = send(cfd, buf, sizeof(buf), 0);
		if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0)
			break;
	}
	
	ret = 0;
cleanup:
	if (sfd > 0) close(sfd); 
	if (cfd > 0) close(cfd);
	
	return ret;
}
