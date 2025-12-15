
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "abc.h"

#define BUF_SIZE 1024

// 接收并发客户端的连接，获取其输入并打印

int main(int argc, char *argv[])
{
	int ret = -1;
    struct addrinfo hints, *result, *rp;
	int sfd, cfd, on, type, maxfd, id, i, toplimit;
	socklen_t typelen = sizeof(type);
    struct sockaddr_storage peer_addr;	// 地址通用结构体
	struct sockaddr_in *addr_ipv4;
    socklen_t peer_addr_len;
    ssize_t nr;
    char buf[BUF_SIZE];
	char ip[32];
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	fd_set rfds, rtfds;
	struct timeval tv;
	uint16_t slen;
	const unsigned LE = 1;
	unsigned isLittleEndian = *((char*)&LE);

	// 参数校验
    if (argc != 2) {
        ECHO(ERRO, "%s [port]", argv[0]);
        goto cleanup;
    }
	
    // 设定协议规则
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    	// 允许IPv4或IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;		// 用作服务器
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
	
    // 获取符合规则的协议地址，存入 addrinfo 结构体中
	if (getaddrinfo(NULL, argv[1], &hints, &result) != 0) {
        ECHO(ERRO, "getaddrinfo: %s", strerror(errno));
		goto cleanup;
	}

	// 选择可用地址绑定
    for (rp = result; rp != NULL; rp = rp->ai_next) {
		if ((sfd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;

		if (getsockopt(sfd, SOL_SOCKET, SO_TYPE, (void*)&type, &typelen) != 0 || type != SOCK_STREAM) {
			close(sfd);
			continue;
		}

		on = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) != 0) {
			close(sfd);
			continue;
		}

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == -1) {
			close(sfd);
			continue;
		}
        if (listen(sfd, 10) == -1) {
			close(sfd);
			continue;
		}
		
        // 在 listen 之后
		if (fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK) == -1) {
			close(sfd);
			continue;
		}

		// 设置 close-on-exec 标志位
		if (fcntl(sfd, F_SETFD, FD_CLOEXEC) == -1) {
			close(sfd);
			continue;
		}
		break;
    }
	if (rp == NULL) {
        ECHO(ERRO, "bind or listen： %s", strerror(errno));
		goto cleanup;
	}

	// 初始化
	FD_ZERO(&rfds);
	FD_ZERO(&rtfds);
	FD_SET(sfd, &rtfds);
	maxfd = sfd;
	toplimit = (int)sizeof(fd_set);

	// 处理连接请求，只作为读服务器使用
	for(;;) {
		// 如果 tv 不为0, select 会更新超时，故每次轮询前重设超时；如果 tv 域为0，则不必重设
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		// select 每次返回时，会对无动作的描述符进行清空，故需对描述集重置
		rfds = rtfds;
		// 对 maxfd 的更新，本程序也可不进行此流程
		int tfd = 0;
		for (id = 0; id <= maxfd; id++) {
			if (FD_ISSET(id, &rfds) && id > tfd)
				tfd = id;
		}
		maxfd = tfd;
		if (maxfd > toplimit) {
			ECHO(ERRO, "fd num exceed the limit(%d > %d)", maxfd, toplimit);
			goto cleanup;
		}
		// 超时 tv 内阻塞在这里
        int res;
		if((res = select(maxfd + 1, &rfds, NULL, NULL, &tv)) == -1) {
            ECHO(ERRO, "select: %s", strerror(errno));
			goto cleanup;
		} else if (res == 0) { // 超时内没内没有描述符动作发生，则不必执行下面的动作
			continue;
		}
		// 遍历所有的描述符，找到动作描述符，进行处理
		for (id = 0; id <= maxfd; id++) {
			if(!FD_ISSET(id, &rfds))
				continue;
            // 1.新连接
			if (id == sfd) {
				peer_addr_len = sizeof(struct sockaddr_storage);
				if ((cfd = (int)accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_len) ) == -1) {
                    ECHO(WARN, "accept failed(%s), but continue...", strerror(errno));
					continue;
				}
				// 获取客户端的信息
				memset(hbuf, 0, NI_MAXHOST);
				memset(sbuf, 0, NI_MAXSERV);
				if (getnameinfo((struct sockaddr *)&peer_addr, peer_addr_len, 
				                hbuf, NI_MAXHOST, sbuf, NI_MAXSERV, NI_NUMERICSERV) != 0) {
                    ECHO(WARN, "getnameinfo failed(%s), but continue...", strerror(errno));
					close(cfd);
					continue;
				} else {
					// 客户端连接成功，加入到监听描述符集中
					ECHO(INFO, "%s:%s connected", hbuf, sbuf);
					FD_SET(cfd, &rtfds);
					if (cfd > maxfd)
						maxfd = cfd;
				}
			}
            // 2. 接收来自客户端的数据
            else {
				memset(&peer_addr, 0, sizeof(peer_addr));
				if (getpeername(id, (struct sockaddr*)&peer_addr, &peer_addr_len) == -1) {
                    ECHO(ERRO, "getpeername: %s", strerror(errno));
					goto cleanup;
				}
				memset(ip, 0, sizeof(ip));
				addr_ipv4 = (struct sockaddr_in *)&peer_addr;
				// 处理接收数据
				memset(buf, 0, BUF_SIZE);
				slen = 0;
				if ((nr = recv(id, buf, BUF_SIZE, 0)) == -1) {
					ECHO(WARN, "recv failed(%s), but continue...", strerror(errno));
					continue;
				} else if (nr == 0) {
					continue;
				}
				if (isLittleEndian) {
					for (i = 0; i < 2; i++)
						slen |= (uint16_t)buf[i] << ((2 - i - 1) << 3);
				} else {
					for (i = 0; i < 2; i++)
						slen |= (uint16_t)buf[i] << (i << 3);
				}
				// 对断开的处理
				if (strcmp(buf + 2, "q") == 0 || strcmp(buf + 2, "Q") == 0 || nr == 0) {
					ECHO(INFO, "%s disconnected", ip);
					close(id);
					FD_CLR(id, &rtfds);
					continue;
				}
				// 打印数据
				if (slen == (uint16_t)strlen(buf + 2)) {
					ECHO(INFO, "%s:%d: %s", inet_ntop(AF_INET, &addr_ipv4->sin_addr, ip, INET_ADDRSTRLEN), ntohs(addr_ipv4->sin_port), buf + 2);
				} else {
					ECHO(ERRO, "received error");
				}
			}
		}
	}

	ret = 0;
cleanup:
	if (result) freeaddrinfo(result);
	memset(&rfds, 0, sizeof rfds);
	memset(&rtfds, 0, sizeof rtfds);
	close(sfd);
	
    return ret;
}