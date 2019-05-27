#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <execinfo.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/syscall.h>

void
mutex_record(pthread_mutex_t * mutex)
{

}


int
pthread_mutex_lock(pthread_mutex_t * mutex)
{
	pid_t ctid = syscall(SYS_gettid);
	printf("thread: %d", ctid);

	int (*pthread_mutex_lockp)(pthread_mutex_t * mutex);
	char * error;
	
	pthread_mutex_lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
	if ((error = dlerror()) != 0x0)
		exit(1);
	int tmp = pthread_mutex_lockp(mutex);

	char buf[50];
	printf("mutex_lock : %p\n", mutex);
	fputs(buf, stderr);

	return tmp ;
}

int
pthread_mutex_unlock(pthread_mutex_t * mutex)
{
	pid_t ctid = syscall(SYS_gettid);
	printf("thread: %d", ctid);

	int (*pthread_mutex_unlockp)(pthread_mutex_t * mutex);
	char * error;

	pthread_mutex_unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	if ((error = dlerror()) != 0x0)
		exit(1);
	int tmp = pthread_mutex_unlockp(mutex);

	char buf[50];
	printf("mutex_unlock : %p\n", mutex);
	fputs(buf, stderr);

	return tmp;
}
