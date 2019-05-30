#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define	NB_THREADS 500

typedef struct
{
  pthread_mutex_t mtx;
  pthread_mutex_t mtx1;
  pthread_mutex_t mtx2;
  int val;
} structos;

void	*thread1_test(void *ptr)
{
  structos *tmp = (structos *)ptr;
  
  pthread_mutex_lock(&tmp->mtx1);
  printf("thread1_test\n");
  pthread_mutex_lock(&tmp->mtx2);
}

void	*thread2_test(void *ptr)
{
  structos *tmp = (structos *)ptr;
  
  pthread_mutex_lock(&tmp->mtx2);
  printf("thread2_test\n");
  pthread_mutex_lock(&tmp->mtx1);
}

int	main(int argc, char *argv[])
{
  pthread_t thread[NB_THREADS];
  pthread_t thread1;
  pthread_t thread2;
  int test = 0;
  int i = 0;
  structos ptr;

  pthread_mutex_init(&ptr.mtx1, NULL);
  pthread_mutex_init(&ptr.mtx2, NULL);
  pthread_create(&thread1, NULL, thread1_test, &ptr);
  pthread_create(&thread2, NULL, thread2_test, &ptr);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  return (0);
}
