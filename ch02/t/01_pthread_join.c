#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* thread_function(void)
{
	int i;
	printf("New thread, thread ID = %u\n", (unsigned)pthread_self());
	for (i = 0; i < 15; i++)
		printf("This is the new thread.\n");
}

int main(void)
{
	pthread_t thread_id;
	int i, ret;
	
	printf("Main thread, thread ID = %u\n", (unsigned)pthread_self());
	ret = pthread_create(&thread_id, NULL, (void*)thread_function, NULL);
	if (ret != 0) {
		printf("Create thread error!\n");
		exit(1);
	}
	
	pthread_join(thread_id, NULL);
	
	for (i = 0; i < 15; i++)
		printf("This is the main thread!\n");
	
	return 0;
}
