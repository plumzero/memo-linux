#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// 父进程定时检测子进程的状态

int main()
{
    pid_t pchild, pwait;
    
    pchild = fork();
    if (pchild == -1) {
        printf("fork failed: %s\n", strerror(errno));
        exit(-1);
    } else if (pchild == 0) {
        printf("child process: %d\n", getpid());
        sleep(10);
        exit(100);
    } else {
        printf("parent process: %d\n", getpid());
        do {
            int status;
            pwait = waitpid(pchild, &status, WNOHANG);
            if (pwait == 0) {
                sleep(1);
                printf("no child process exited! continue check ...\n");
            }
        } while (pwait == 0);
        if (pwait == pchild) {
            printf("successfully get child %d\n", pwait);
        } else {
            printf("some error occured\n");
        }
    }
    
    return 0;
}
