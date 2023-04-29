
主要是以 C/S 形式示例几种 I/O 模型的使用方法。

一般来说，客户端套接字不需要设置为非阻塞，但如果要使客户端能够处理一些中断信号，那么可以使用非阻塞的方式实现，不过此时最好还要再结合 I/O 多路复用来实现。

epoll_wait 每次最多只返回一个事件，这在高并发情况下有些不利。

### 参考

* [浅谈TCP/IP网络编程中socket的行为](https://www.cnblogs.com/junneyang/p/6126635.html)


### 内容

* [socket](01_socket)
* [select](02_select)
* [poll](03_poll)
