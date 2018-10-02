#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int thread_count;
int flag = 0;
long long n;
double sum = 0;
pthread_mutex_t mutex;

void* Thread_sum(void* rank){
  long my_rank = (long) rank;
  double factor, my_sum = 0.0;
  long long i;
  long long my_n = n/thread_count;
  long long my_first_i = my_n * my_rank;
  long long my_last_i = my_first_i + my_n;

  if(my_first_i % 2 == 0) factor = 1.0;
  else factor = -1.0;

  for(i = my_first_i; i < my_last_i; i++, factor = -factor){
    my_sum += factor/(2*i+1);
  }

  pthread_mutex_lock(&mutex);
  sum += my_sum;
  pthread_mutex_unlock(&mutex);

  return NULL;

}

int main(int argc, char* argv[]){
  long thread;
  pthread_t* thread_handles;
  struct timespec begin, end;
  double elapsed;
  clock_gettime(CLOCK_MONOTONIC, &begin);
  pthread_mutex_init(&mutex,NULL);
  scanf("%lld", &n);
  //n=16;
  thread_count = strtol(argv[1], NULL, 10);
  thread_handles = malloc (thread_count* sizeof(pthread_t));
  clock_gettime(CLOCK_MONOTONIC, &begin);
  for(thread = 0; thread < thread_count; thread++) pthread_create(&thread_handles[thread], NULL, Thread_sum, (void*) thread);
  for(thread = 0; thread < thread_count; thread++) pthread_join(thread_handles[thread], NULL);
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec)/1000000000.0;
  printf("%f %f\n", 4*sum, elapsed);
  pthread_mutex_destroy(&mutex);
  free(thread_handles);
  return 0;
}