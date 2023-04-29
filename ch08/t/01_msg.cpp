
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static bool _running = true;

void signal_handler(int signo)
{
    _running = false;
    printf("process %d catch signal %d and exit\n", getpid(), signo);
    // exit(0); // 也可以用这个
}

// 参考: https://blog.csdn.net/weixin_44920742/article/details/123305262

int main()
{
	pid_t pid;
    int id = 1;

    signal(SIGINT,  signal_handler);

	if ((pid = fork()) < 0) {
		perror("fork error");
	} else if (pid == 0) {
        int qid_snd = msgget(id, IPC_CREAT | 0666);
        printf("child process(%d), parent process(%d), queue id(%d)\n", getpid(), getppid(), qid_snd);

        while (_running) {
            char sbuf[64] = { 0 };
            int n = snprintf(sbuf, sizeof(sbuf), "Hello %d", getpid());
            int ret = msgsnd(qid_snd, sbuf, n, IPC_NOWAIT);
            if (ret < 0 && EINTR != errno) {
                perror("msgsnd error");
                exit(-1);
            }
            printf("process %d send buf(%ld): %s\n", getpid(), strlen(sbuf), sbuf);
            sleep(1);
        }
        if (qid_snd > 0) {
            msgctl(qid_snd, IPC_RMID, nullptr);
        }
        // sleep(2);
	} else {
        int qid_rcv = msgget(id, IPC_CREAT | 0666);
        printf("parent process(%d), queue id(%d)\n", getpid(), qid_rcv);

        while (_running) {
            char rbuf[64] = { 0 };
            int ret = msgrcv(qid_rcv, (void *)rbuf, sizeof(rbuf), 0, 0);
            if (ret < 0 && EINTR != errno) {
                perror("msgrcv error");
                exit(-1);
            }
            printf("process %d recv buf(%ld): %s\n", getpid(), strlen(rbuf), rbuf);
        }

        if (qid_rcv > 0) {
            msgctl(qid_rcv, IPC_RMID, nullptr);
        }
        // sleep(2);
	}

    return 0;
}