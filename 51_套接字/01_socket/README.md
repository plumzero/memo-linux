
依次启动服务端和客户端，然后你一句，我一句...

### 测试组一

套接字测试。

* 服务端: socket_server.c
* 客户端: socket_client.c


### 测试组二

 msgpeek_server 作为服务端, msgpeek_client 作为客户端, 测试时客户端发送数据，服务端接收数据服务端接收数据时，先'窥'一下从消息头中获取有效荷载长度，再从底层缓存中抽取对应长度数据。

* 服务端: msgpeek_server.c
* 客户端: msgpeek_client.c
