
### 进程与信号

1. 人们往往将信号称为"软件中断"。进程收到信号，就意味着某一事件或异常的发生。
2. 内核、其他进程(只要具有相应的权限)或进程自身均可向进程发送信号。
3. 发生下列情况时，内核可向进程发送信号:
  - 用户键入中断字符(control-C)
  - 进程的子进程之一已经终止
  - 由进程设定的定时器已经到期
  - 进程尝试访问无效的内存地址
4. 在 shell 中，可使用 kill 命令向进程发送信号。在程序内部，系统调用 `kill()` 可提供相同的功能。
5. 收到信号时，进程会根据信号采取如下动作之一:
  - 忽略信号
  - 被信号"杀死"
  - 先挂起，之后再被专用信号唤醒
6. 就大多数信号类型而言，程序可选择不采取默认的信号动作，而是忽略信号或者建立自己的信号处理器。信号处理器是程序员定义的函数，会在进程收到信号时自动调用，根据信号的产生条件执行相应的动作。

## 信号阻塞的含义
- 信号阻塞表示信号可以被进程或线程捕获，并做出相应的处理(三种处理方法)；
- linux 将信号添加到一个信号集(set)中，将通过设置how(SIG_BLOCK, SIG_UNBLOCK,
  SIG_SETMASK)对该信号集作为相应的处理。
- 对信号集的初始化操作有两种: 从信号集中清除信号或向信号集中添加信号。

## 从信号集中清除信号
- 开始时将所有信号都添加到信号集中，之后从该信号集中删除某个信号，并为号集设置动作。
- 代码示例如下
  ```c
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    assert(pthread_sigmask(SIG_BLOCK, &set, nullptr) == 0);
  ```
  上述代码中将所有信号都设置到信号集中，之后 SIGINT 从信号集中清除，并设置信号集动作
  为阻塞(SIG_BLOCK)。这样的话，该进程或线程就无法捕获到除 SIGINT 之外的其他信号。

## 向信号集中添加信号
- 开始时将信号集清空，之后将信号添加到信号集中，并设置动作。
- 代码示例如下:
  ```c
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    assert(pthread_sigmask(SIG_BLOCK, &set, nullpt) == 0);
  ```
  上述代码中将 SIGINT 添加到信号集中，之后设置信号集动作为阻塞(SIG_BLOCK)。这样的话，
  该进程或线程就无法捕获到 SIGINT 信号。

## SIG_UNBLOCK 和 SIG_SETMASK 分别与 SIG_BLOCK 比较
- SIG_UNBLOCK 对信号集 set 执行和 SIG_BLOCK 相反的动作；
- SIG_SETMASK 也是将信号集 set 中的信号设置为阻塞。不同的是，SIG_BLOCK 是将信号集 set
  中的信号添加到(位或)原有的阻塞信号集中，而 SIG_SETMASK 是将信号集 set 覆盖原有的阻塞
  信号集。
  
## 对信号集操作相关的函数
- 常用函数
    + sigfillset
    + sigdelset
    + sigemptyset
    + sigaddset
    + sigismember
    + sigprocmask
    + sigwait
- sigwait 函数说明
    + 定义
      ```c
        int sigwait(const sigset_t *set, int *sig);
      ```
    + 监听信号集中所包含的信号，并将其中触发的信号记入 sig 中；
    + sigwait 对信号集的操作是只读的

## ANSI C 信号处理句柄
- 定义
  ```c
    typedef void (*sighandler_t)(int);
    sighandler_t signal(int signum, sighandler_t handler);
  ```
- 描述
    + 该句柄行为会因 UNIX 版本不同而异，Linux 也是。可以通过 sigaction 进行替代来避免这种差异。
    + 如果信号 signum 传入进程，会触发下列情形的一种:
        + 如果配置设置为 SIG_IGN, 则该信号被忽略；
        + 如果配置设置为 SIG_DFL, 执行该信号对应的缺省动作；
        + 如果配置设置有信号处理句柄，则调用参数为 signum 的信号处理句柄
    + SIGKILL 和 SIGSTOP 信号不能被抓取、阻塞或忽略。
- 返回值
    + 失败时返回 SIG_ERR
- 一般调用方式
  ```c
    assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);
  ```