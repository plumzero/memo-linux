#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define SIZE 256

struct message {
	long int msg_type;
	char msg_text[SIZE];
};

/**
 * 进程间通过消息队列通信，主要是:
 *   创建或打开消息队列
 *   向消息队列中添加消息
 *   从消息队列中读取消息
 *   控制消息队列
 */

int main(int argc, char *argv[])
{
	int qid;
	key_t key;
	int len;
	struct message msg;
	
	if ((key = ftok(".", 'a')) == -1) {   // ftok() 用来产生标准的 key
		printf("ftok failed\n");
		exit(1);
	}
	if ((qid = msgget(key, IPC_CREAT|0666)) == -1) {
		printf("msgget faild\n");
		exit(1);
	}
	printf("创建、打开的队列号是: %d\n", qid);
	printf("请输入要加入到队列中的内容: \n");
	if ((fgets(msg.msg_text, SIZE, stdin)) == NULL) {
		printf("fgets failed\n");
		exit(1);
	}
	msg.msg_type = getpid();
	len = strlen(msg.msg_text);
	if (msgsnd(qid, &msg, len, 0) < 0) {
		printf("msgsnd faild\n");
		exit(1);
	}
	memset(&msg, 0, sizeof(msg));
	if(msgrcv(qid, &msg, SIZE, 0, 0) < 0) {
		printf("msgrcv failed\n");
		exit(1);
	}
	printf("读取的消息是: %ld %s\n", msg.msg_type, msg.msg_text);
	if (msgctl(qid, IPC_RMID, NULL) < 0) {	// 删除消息队列
		printf("msgctl(IPC_RMID) failed");
		exit(1);
	}
	exit(0);
}