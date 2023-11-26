#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

char message[] = "Hello World!";
int thread_finished = 0;

void* thread_function(void *arg)
{
	printf("New thread is running, argument is %s\n", (char*)arg);
	sleep(4);
	printf("New thread setting finished flag, and exiting now.\n");
	thread_finished = 1;
	pthread_exit(NULL);
}

int main()
{
	pthread_t thread_id;
	int ret;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret != 0)
	{
		printf("Attribute init error!\n");
		exit(1);
	}
	
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0)
	{
		printf("Setting detached attribute error!\n");
		exit(1);
	}
	
	ret = pthread_create(&thread_id, &attr, (void*)thread_function, (void*)message);
	if (ret != 0)
	{
		printf("Create thread error!\n");
		exit(1);
	}
	
	pthread_attr_destroy(&attr);
	
	while(!thread_finished)
	{
		printf("Main thread, waiting for other thread finished...\n");
		sleep(1);
	}
	printf("Other thread finished, bye!\n");
	
	return 0;
}
