#include <signal.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

class MyClass
{
public:
    int val = 0;
    void print() { std::cout << val << std::endl; }
};

int main(int argc,char**argv)
{
    MyClass obj;
    obj.val = 104;

    std::cout << "addr: " << &obj << std::endl;
	
    // 信号处理
	union sigval mysig;
	mysig.sival_ptr = (void*)&obj;

    struct sigaction act;
	sigemptyset(&act.sa_mask);
    // 三参数信号处理函数
	act.sa_sigaction = [](int signum, siginfo_t *info, void* arg) {
        std::cout << "pointer: " << arg << std::endl;
        MyClass* ptr = (MyClass*)((*info).si_ptr);
        ptr->print();
        ptr->val += 1;
        std::cout << "lambda addr: " << ptr << std::endl;
        std::cout << "lambda signum: " << signum << std::endl;
        // exit(-1);
    };
    // 信息传递开关
	act.sa_flags = SA_SIGINFO;

	if(sigaction(SIGINT, &act, nullptr) < 0) {
        std::cout << "install signal error" << std::endl;
        return -1;
	}
	
	while(1) {
		sleep(1);
        std::cout << "wait for the signal" << std::endl;
		sigqueue(getpid(), SIGINT, mysig);          // 向本进程发送信号，并传递附加信息
	}

    return 0;
}
