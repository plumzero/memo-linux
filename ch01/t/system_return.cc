
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

// 提供一种对 system 调用返回值判断的参考

int main()
{
	int status;
	for (int i = 0; i < 5; i++) {
		status = system("ls -l");
		if (status < 0 || (status == 127 && errno != 0)) {
            printf("system error! %d\n", status);
        }
        
        sleep(1);
	}

    return 0;
}