#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

char buffer[] = "Hello World";

void* thread_function(void *arg)
{
	int i ;
	printf("NEW thread is running, argument is %s\n", (char*)arg);
	strcpy(buffer, "Bye!");
	pthread_exit("I like Linux C program!");
}

int main(void)
{
	pthread_t thread_id;
	int ret;
	void *thread_result;
	
	printf("Main thread is running!\n");
	printf("Before new thread running, the buffer content is %s\n", buffer);
	
	ret = pthread_create(&thread_id, NULL, (void*)thread_function, (void*)buffer);
	if (ret != 0) {
		printf("Create thread error!\n");
		exit(1);
	}
	
	ret = pthread_join(thread_id, &thread_result);	// �������̣߳��ȴ����߳�ִ�����
	if (ret != 0) {
		printf("Thread join error!\n");
		exit(1);
	}
	printf("Main waiting new thread, it returns %s\n", (char*)thread_result);
	printf("New thread returned, now the bufferis %s\n", buffer);
	
	return 0;
}
