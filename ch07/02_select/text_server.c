#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/select.h>
#include <netinet/tcp.h>

#include "abc.h"

#define BUF_SIZE 1024

// 接收一个客户端文本完毕后，必须要清除该客户端所对应的文件描述符，否则服务端无法打印后续客户端文本，原因暂未知

int main(int argc, char *argv[])
{
	int ret = 0;
    struct addrinfo hints, *result, *rp;
	int sfd, cfd, on, type, maxfd, id, i, toplimit;
	socklen_t typelen = sizeof(type);
    struct sockaddr_storage peer_addr;				// 地址通用结构体，用于保存客户端地址
    socklen_t peer_addr_len;
    ssize_t nr;
    char buf[BUF_SIZE];
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	fd_set rfds, rtfds;
	struct timeval tv;
	uint16_t slen;
	const unsigned LE = 1;
	unsigned isLittleEndian = *((char*)&LE);

    if (argc != 2) {
        ECHO(ERRO, "%s [ip] [port]", argv[0]);
        exit(-1);
    }

	// 设置地址规则
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    	// 允许IPv4或IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    	// 用作服务器
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
	// 本地获取
	if ((ret = getaddrinfo(NULL, argv[1], &hints, &result)) != 0) {
        ECHO(ERRO, "getaddrinfo: %s", gai_strerror(ret));
		goto cleanup;
	}
	// 尝试 result 中的每个地址，直到可以 bind 并 listen
	ret = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
		if ((sfd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;
		// 确认绑定到该端口的套接字类型是TCP
		if (getsockopt(sfd, SOL_SOCKET, SO_TYPE, (void*)&type, &typelen) != 0 || type != SOCK_STREAM) {
			close(sfd);
			continue;
		}
		// 设置端口可复用。服务器因意外断开，绑定端口未释放，重启此服务器可立即再使用该端口
		on = 1;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on)) != 0) {
			close(sfd);
			continue;
		}
		// 绑定
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == -1) {
			close(sfd);
			continue;
		}
		// 监听
        if (listen(sfd, 10) == -1) {
			close(sfd);
			continue;
		}
		// 设置监听端口非阻塞（通常，设置监听套接为非阻塞操作在 bind listen 完成之后进行）
		if (fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK) == -1) {
			close(sfd);
			continue;
		}
		// 设置 close-on-exec 标志位
		if (fcntl(sfd, F_SETFD, FD_CLOEXEC) == -1) {
			close(sfd);
			continue;
		}
		ret = 0;
		break;
    }
	if (ret) {
        ECHO(ERRO, "bind or listen: %s", strerror(errno));
		goto cleanup;
	}
	// 初始化描述符集
	FD_ZERO(&rfds);
	FD_ZERO(&rtfds);
	FD_SET(sfd, &rtfds);
	maxfd = sfd;
	toplimit = (int)sizeof(fd_set);
	// 处理连接请求，只作为读服务器使用
	for(;;) {
		// 如果 tv 不为零, select 会更新超时，故每次轮询前重设超时；如果 tv 域为0，则不必重设
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		// select 每次返回时，会对无动作的描述符进行清空，故需对描述集重新预置
		rfds = rtfds;
		// 对 maxfd 的更新，本程序也可不进行此流程
		int tfd = 0;
		for (id = 0; id <= maxfd; id++) {
			if (FD_ISSET(id, &rfds) && id > tfd)
				tfd = id;
		}
		maxfd = tfd;
		if (maxfd > toplimit) {
			ECHO(ERRO, "maxfd(%d) exceed its toplimit(%d)", maxfd, toplimit);
			goto cleanup;
		}
		// 超时 tv 内阻塞在这里
		if((ret = select(maxfd + 1, &rfds, NULL, NULL, &tv)) == -1) {
            ECHO(ERRO, "select: %s", strerror(errno));
			goto cleanup;
		} else if (ret == 0) {
			continue;	// 超时内没内没有描述符动作发生，则不必执行下面的动作
		}
		// 遍历所有的描述符，找到动作描述符，处理
		for (id = 0; id <= maxfd; id++) {
			if(!FD_ISSET(id, &rfds))
				continue;
			// 两种动作，来自新客户端的连接或已有客户端的数据
			if (id == sfd) {
				// 接受连接
				peer_addr_len = sizeof(struct sockaddr_storage);
				if ((cfd = (int)accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_len) ) == -1) {
					ECHO(ERRO, "accept: %s", strerror(errno));
					continue;
				}
				// 设置连接端口非阻塞
				if (fcntl(cfd, F_SETFL, fcntl(cfd, F_GETFL) | O_NONBLOCK) == -1) {
					ECHO(ERRO, "fcntl: %s", strerror(errno));
					close(cfd);
					continue;
				}
				// 获取客户端的信息
				memset(hbuf, 0, NI_MAXHOST);
				memset(sbuf, 0, NI_MAXSERV);
				if ((ret = getnameinfo((struct sockaddr *)&peer_addr, peer_addr_len, hbuf, NI_MAXHOST, sbuf, NI_MAXSERV, NI_NUMERICSERV)) != 0) {
					ECHO(ERRO, "getnameinfo: %s", gai_strerror(ret));
					close(cfd);
					continue;
				} else {
					// 加入到总读描述符集中
					ECHO(INFO, "%s:%s connected", hbuf, sbuf);
					FD_SET(cfd, &rtfds);
					if (cfd > maxfd)
						maxfd = cfd;
				}
			} else {
				memset(buf, 0, BUF_SIZE);
				slen = 0;
				if ((nr = recv(id, buf, BUF_SIZE, 0)) == -1) {
					fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
					goto cleanup;
				}
				if (nr == 0)
					break;
				// 网络序转主机序
				if (isLittleEndian) {
					for (i = 0; i < 2; i++)
						slen |= (uint16_t)buf[i] << ((2 - i - 1) << 3);
				} else {
					for (i = 0; i < 2; i++)
						slen |= (uint16_t)buf[i] << (i << 3);
				}
				if (slen == (uint16_t)strlen(buf + 2)) {
					if (slen == 1 && buf[2] == 0x04) {	// 清除套接字
						ECHO(INFO, "Received over");
						close(id);
						FD_CLR(id, &rtfds);
						break;
					} else {
						ECHO(INFO, "RECV (%2d bytes): %s", slen, buf + 2);
					}
				} else {
					ECHO(ERRO, "Received error");	// 接收错误时，暂不作处理
					continue;
				}
			}
		}
	}

cleanup:
	// 释放
	if (result) freeaddrinfo(result);
	memset(&rfds, 0, sizeof rfds);
	memset(&rtfds, 0, sizeof rtfds);
	close(sfd);

	return ret;
}