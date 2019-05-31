#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <execinfo.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/syscall.h>

#define TRUE 1
#define FALSE 0
#define NUMOFMUTEX 10
#define NUMOFTHREAD 10

static pthread_t threads[NUMOFTHREAD];
static int thread_head = 0;

static pthread_mutex_t * mutexes[NUMOFMUTEX];
static int mutex_head = 0;

static int graph[NUMOFMUTEX][NUMOFMUTEX] = {
    [0 ... NUMOFMUTEX-1] = { [0 ... NUMOFMUTEX-1] = 0 }
};

static int stack[NUMOFTHREAD][NUMOFMUTEX] = {
    [0 ... NUMOFTHREAD-1] = { [0 ... NUMOFMUTEX-1] = -1 }
};

int
isCyclicUtil(int v, int visited[], int *recStack)
{
    if(visited[v] == FALSE)
    {
        visited[v] = TRUE;
        recStack[v] = TRUE;
        
        for(int i=0; i<NUMOFMUTEX; i++){
            if(graph[v][i] == 1)
            {
                if(!visited[i] && isCyclicUtil(i, visited, recStack))
                    return TRUE;
                else if (recStack[i])
                    return TRUE;
            }
        }
        
    }
    recStack[v] = FALSE;
    return FALSE;
}

// This function is a variation of DFS() in https://www.geeksforgeeks.org/archives/18212
int
isCyclic()
{
    int visited[NUMOFMUTEX] = { [0 ... NUMOFMUTEX-1] = 0 };
    int recStack[NUMOFMUTEX] = { [0 ... NUMOFMUTEX-1] = 0 };
    
    for(int i = 0; i < NUMOFMUTEX; i++)
        if (isCyclicUtil(i, visited, recStack))
            return TRUE;
    
    return FALSE;
}


void
mk_edge(int from, int to)
{
    graph[from][to] = 1;
}

void
rm_edge(int from, int to)
{
    graph[from][to] = 0;
}

void
file_print_lock_graph()
{
    FILE * fp;
    fp = fopen("dmonitor.trace", "w");
    for(int i=0; i<NUMOFMUTEX; i++)
    {
        for(int j=0; j<NUMOFMUTEX; j++)
        {
            fprintf(fp, "%d ", graph[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void
file_print_mutex_info(pthread_mutex_t * mutex, pthread_t thread)
{
    FILE * fp;
    fp = fopen("dmonitor.trace", "a");
    fprintf(fp, "pthread_mutex_lock: mutex(%p), thread(%d)\n", mutex, thread);
    fclose(fp);
}

void
file_print_stack_trace(size_t sz)
{
    FILE * fp;
    fp = fopen("dmonitor.trace", "a");
    
    void * arr[10];
    char ** stack = backtrace_symbols(arr, sz);

    fprintf(fp, "stack trace\n");
    fprintf(fp, "============\n");
    for(int i=0; i<sz; i++)
    {
        fprintf(fp, "[%d] %s\n", i, stack[i]);
    }
    fprintf(fp, "============\n");

    fclose(fp);
} 


int
pthread_mutex_lock(pthread_mutex_t * mutex)
{
    pthread_t cur_thread = pthread_self();
    int cur_thread_index = -1;
    for(int i=0; i<NUMOFTHREAD; i++)
    { 
        if(threads[i] == cur_thread)
        {
            cur_thread_index = i;
            break;
        }
    }
    if(cur_thread_index == -1) 
    {
        threads[thread_head] = cur_thread;
        cur_thread_index = thread_head;
        thread_head++;
    }

    int cur_mutex_index = -1;
    for(int i=0; i<NUMOFMUTEX; i++) 
    {
        if(mutexes[i] == mutex) 
        {
            cur_mutex_index = i;
            break;
        }
    }
    if(cur_mutex_index == -1) 
    {
        mutexes[mutex_head] = mutex;
        cur_mutex_index = mutex_head;
        mutex_head++;
    }

    for(int i=0; i<NUMOFMUTEX; i++)
    {
        if(stack[cur_thread_index][i] != -1)
        {
            mk_edge(stack[cur_thread_index][i], cur_mutex_index);
        }
        else
        {
            stack[cur_thread_index][i] = cur_mutex_index;
            break;
        }
    }

    static __thread int n_mutex = 0;
    n_mutex += 1;

    int cycle = isCyclic();
    if(cycle == TRUE && n_mutex == 1) {
        file_print_lock_graph();
        file_print_mutex_info(mutex, cur_thread);
        void * arr[10];
        size_t sz = backtrace(arr, 10);
        file_print_stack_trace(sz);
    }

    n_mutex -= 1;
 
    int (*pthread_mutex_lockp)(pthread_mutex_t * mutex);
    char * error;
	
    pthread_mutex_lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    if ((error = dlerror()) != 0x0)
        exit(1);
    int tmp = pthread_mutex_lockp(mutex);

    char buf[50];
//    printf("thread: %d index: %d", cur_thread, cur_thread_index);
//    printf("@@mutex_lock : %p index: %d\n", mutex, cur_mutex_index);
    fputs(buf, stderr);

    return tmp ;
}

int
pthread_mutex_unlock(pthread_mutex_t * mutex)
{
    pthread_t cur_thread = pthread_self();
    int cur_thread_index;
    for(int i=0; i<NUMOFTHREAD; i++)
    {
        if(threads[i] == cur_thread)
        {
            cur_thread_index = i;
            break;
        }
    }
    
    int cur_mutex_index;
    for(int i=0; i<NUMOFMUTEX; i++)
    {
        if(mutexes[i] == mutex)
        {
            cur_mutex_index = i;
            break;
        }
    }

    for(int i=0; i<NUMOFMUTEX; i++)
    {
        if(stack[cur_thread_index][i] != cur_mutex_index)
        {
//            rm_edge(stack[cur_thread_index][i], cur_mutex_index);
        }
        else
        {
            stack[cur_thread_index][i] = -1;
            break;
        }
    }

    int (*pthread_mutex_unlockp)(pthread_mutex_t * mutex);
    char * error;

    pthread_mutex_unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    if ((error = dlerror()) != 0x0)
        exit(1);
    int tmp = pthread_mutex_unlockp(mutex);

    char buf[50];
//    printf("thread: %d index: %d", cur_thread, cur_thread_index);
//    printf("mutex_unlock : %p index: %d\n", mutex, cur_mutex_index);
    fputs(buf, stderr);

    return tmp;
}
