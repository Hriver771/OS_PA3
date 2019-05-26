#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

int
pthread_mutex_lock(pthread_mutex_t * mutex_add)
{
	int (*pthread_mutex_lockp)(pthread_mutex_t * mutex_add);

	char * error;
	
	pthread_mutex_lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1);
	int tmp = pthread_mutex_lockp(mutex_add);

	char buf[50];
	printf("mutexlock : (%p)\n", mutex_add);
	fputs(buf, stderr);

	return tmp ;
}
