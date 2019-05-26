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
	int (*pthread_mutex_lock_p)(pthread_mutex_t *);
	char * error;
	
	pthread_mutex_lock_p = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1);
	int tmp = pthead_mutex_lock_p(mutex_add);

	char buf[50];
	printf("mutexlock : (%s)\n", *mutex_add);
	fputs(buf, stderr);

	return tmp ;
}
