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

#define TRUE 1
#define FALSE 0
#define NUMOFMUTEX 100
#define NUMOFTHREAD 10

//static pid_t threads[NUMOFTHREAD];
static pthread_t threads[NUMOFTHREAD];
//static LockStack * locksperthread[10]; 
static int head = 0;

static int graph[NUMOFMUTEX][NUMOFMUTEX] = {
	[0 ... NUMOFMUTEX-1] = { [0 ... NUMOFMUTEX-1] = 0 }
};

int visit[NUMOFMUTEX];

int
is_cycle_dfs(int node)
{
    visit[node] = 1;
    if(node < NUMOFMUTEX) {
        for(int i=0; i<NUMOFMUTEX; i++) {
            if(graph[node][i] == 1) {
                if(visit[i] == 1)
                    return 1;
                if(is_cycle_dfs(i) == 1)
                    return 1;
            }
        }
    }
}

void
my_mk_edge()
{

}

void
my_rm_edge()
{

}




int
pthread_mutex_lock(pthread_mutex_t * mutex)
{
//    pid_t ctid = syscall(SYS_gettid);
    pthread_t ctid = pthread_self();
    int ctindex = -1;
    for(int i=0; i<NUMOFTHREAD; i++){
        if(threads[i] == ctid){
            ctindex = i;
        }
    }
    if(ctindex == -1) {
        ctindex = head;
        threads[head] = ctid;
        head++;
    }
   // printf("thread: %d index: %d", ctid, ctindex);

    int (*pthread_mutex_lockp)(pthread_mutex_t * mutex);
    char * error;
	
    pthread_mutex_lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    if ((error = dlerror()) != 0x0)
        exit(1);
    int tmp = pthread_mutex_lockp(mutex);

    char buf[50];
    printf("thread: %d index: %d", ctid, ctindex);
    printf("@@mutex_lock : %p\n", mutex);
    fputs(buf, stderr);

    return tmp ;
}

int
pthread_mutex_unlock(pthread_mutex_t * mutex)
{
//    pid_t ctid = syscall(SYS_gettid);
    pthread_t ctid = pthread_self();
    int ctindex;
    for(int i=0; i<NUMOFTHREAD; i++){
        if(threads[i] == ctid){
            ctindex = i;
        }
    }
//    printf("thread: %d", ctid);

    int (*pthread_mutex_unlockp)(pthread_mutex_t * mutex);
    char * error;

    pthread_mutex_unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    if ((error = dlerror()) != 0x0)
        exit(1);
    int tmp = pthread_mutex_unlockp(mutex);

    char buf[50];
    printf("thread: %d index: %d", ctid, ctindex);
    printf("mutex_unlock : %p\n", mutex);
    fputs(buf, stderr);

    return tmp;
}
