
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static bool _running = true;

void signal_handler(int signo)
{
    _running = false;
    printf("process %d catch signal %d and exit\n", getpid(), signo);
    exit(0);
}

#define SND_BUF_SIZE 64
#define RCV_BUF_SIZE 64
#define MQ_MSG_SIZE  64

// 需满足 RCV_BUF_SIZE >= MQ_MSG_SIZE && SND_BUF_SIZE <= MQ_MSG_SIZE

int main()
{
	pid_t pid;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 16;
    attr.mq_msgsize = MQ_MSG_SIZE;
    attr.mq_curmsgs = 0;

    int oflag = O_CREAT | O_RDWR;
    int mode = 0664;

    const char* qname = "/hello";

    signal(SIGINT,  signal_handler);

	if ((pid = fork()) < 0) {
		perror("fork error");
	} else if (pid == 0) {
        mqd_t qid_snd = mq_open(qname, oflag, mode, nullptr);
        if (qid_snd < 0 && EEXIST != errno) {
            perror("mq_open error");
            exit(-1);
        }

        printf("child process(%d), parent process(%d), queue id(%d)\n", getpid(), getppid(), qid_snd);

        while (_running) {
            char sbuf[SND_BUF_SIZE] = { 0 };
            int n = snprintf(sbuf, sizeof(sbuf), "Hello %d", getpid());
            int ret = mq_send(qid_snd, sbuf, n, 1);
            if (ret != 0) {
                perror("mq_send error");
                exit(-1);
            }

            printf("process %d send buf(%ld): %s\n", getpid(), strlen(sbuf), sbuf);
            sleep(1);
        }
        if (qid_snd > 0) {
            mq_close(qid_snd);
        }
        // sleep(2);
	} else {
        mqd_t qid_rcv = mq_open(qname, oflag, mode, &attr);
        if (qid_rcv < 0 && EEXIST != errno) {
            perror("mq_open error");
            exit(-1);
        }

        printf("parent process(%d), queue id(%d)\n", getpid(), qid_rcv);

        while (_running) {
            char rbuf[RCV_BUF_SIZE] = { 0 };
            unsigned int prio;
            int ret = mq_receive(qid_rcv, rbuf, sizeof(rbuf) - 1, &prio);
            if (ret < 0 && EINTR != errno) {
                perror("mq_receive error");
                exit(-1);
            }
            printf("process %d recv buf(prio=%d,%ld): %s\n", getpid(), prio, strlen(rbuf), rbuf);
        }

        if (qid_rcv > 0) {
            mq_close(qid_rcv);
        }
        // sleep(2);
	}

    return 0;
}