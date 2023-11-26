#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// system 对调用它的进程会形成阻塞，fork 出一个子进程调用它

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(-1);
    }

    if (pid == 0) {
        int status = system("sleep 6");
        if (status < 0) {
            printf("child call system: unexpected result\n");
            exit(-1);
        }
        exit(5);
    } else {
        int stat_val;
        waitpid(pid, &stat_val, 0);
        if (WIFEXITED(stat_val)) {  // 子进程正常终止， WIFEXITED 取出的字段值非零
            printf("child exited with code %d\n", WEXITSTATUS(stat_val));   // 打印退出状态
        } else {                    // 子进程异常终止， WIFSIGNALED 取出的字段值非零
            printf("child terminated abnormally, signal %d\n", WTERMSIG(stat_val)); // 打印信号编号
        }
    }

    return 0;
}