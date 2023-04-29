
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

class Singleton
{
public:
    static Singleton* getInstance();
    static void destroyInstance();
protected:
    static Singleton* instance;
public:
    int i;
    int j;
};

Singleton* Singleton::instance = nullptr;

Singleton* Singleton::getInstance()
{
    if (nullptr == instance) {
        instance = new Singleton();
    }
    return instance;
}

void Singleton::destroyInstance()
{   
    if (instance) {
        delete instance;
    }
    
    instance = nullptr;
}

/** 多进程下每个进程的单例互不影响 */

int main()
{
    int var;
    pid_t pid;
    var = 88;
    
    Singleton::getInstance()->i = 10;
    Singleton::getInstance()->j = 100;
    std::cout << getpid() << ": i=" << Singleton::getInstance()->i << std::endl;
    std::cout << getpid() << ": j=" << Singleton::getInstance()->j << std::endl;

    if ((pid = fork()) < 0)
    {
        std::cerr << "fork error" << std::endl;
    } else if (pid == 0) {
        Singleton::getInstance()->i = 15;
        sleep(2);
        std::cout << getpid() << ": i=" << Singleton::getInstance()->i << std::endl;
        std::cout << getpid() << ": j=" << Singleton::getInstance()->j << std::endl;
    } else {
        Singleton::getInstance()->j = 150;
        sleep(2);
        std::cout << getpid() << ": i=" << Singleton::getInstance()->i << std::endl;
        std::cout << getpid() << ": j=" << Singleton::getInstance()->j << std::endl;
    }
    
    return 0;
}
