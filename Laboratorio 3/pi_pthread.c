#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;
int flag = 0;
long long n;
double sum = 0;

void* Thread_sum(void* rank){
  long my_rank = (long) rank;
  double factor, my_sum = 0.0;
  long long i;
  long long my_n = n/thread_count;
  long long my_first_i = my_n * my_rank;
  long long my_last_i = my_first_i + my_n;

  if(my_first_i % 2 == 0){
    factor = 1.0;
  }
  else{
    factor = -1.0;
  }

  for(i = my_first_i; i < my_last_i; i++, factor = -factor){
    while(flag != my_rank);
    sum += factor/(2*i+1);
    //my_sum += factor/(2*i+1);
    flag = (flag+1) % thread_count;
  }

  //while(flag != my_rank);
  //sum += my_sum;
  //flag = (flag+1) % thread_count;

  return NULL;
}

int main(int argc, char* argv[]){

	long thread;
  pthread_t* thread_handles;
  struct timespec inicio,fin;
  double tiempo;

  scanf("%lld",&n);
  thread_count = strtol(argv[1],NULL,10);
	thread_handles=malloc(thread_count*sizeof(pthread_t));

  clock_gettime(CLOCK_MONOTONIC,&inicio);
  for(thread=0;thread<thread_count;thread++){
    pthread_create(&thread_handles[thread],NULL,Thread_sum,(void*)thread);
  }
	for(thread=0;thread<thread_count;thread++){
    pthread_join(thread_handles[thread],NULL);
  }
  clock_gettime(CLOCK_MONOTONIC,&fin);

  tiempo=fin.tv_sec-inicio.tv_sec;
  tiempo=tiempo+ (fin.tv_nsec - inicio.tv_nsec)/1000000000.0;
  printf("%f  %f\n",4*sum,tiempo);

	free(thread_handles);
  return 0;
}