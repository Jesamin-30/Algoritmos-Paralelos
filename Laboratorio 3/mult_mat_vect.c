#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count,m,n;
double **A,*B,*resultado;
void* pth_mat_vect(void *rank){
	long my_rank=(long) rank;
	int local_m=m/thread_count;
	int my_first_row=my_rank*local_m;
	int my_last_row=(my_rank+1)*local_m-1;

	for(int i=my_first_row;i<=my_last_row;i++){
		resultado[i]=0;
		for(int j=0;j<n;j++){
			resultado[i]=resultado[i]+A[i][j]*B[j];
		}
	}
	return NULL;
}

int main(int argc, char const *argv[]){
	int i,j;
  long thread;
  pthread_t* thread_handles;
  double elapsed;
  struct timespec begin,end;
  n=16;
  m=100;
  A = (double**) malloc(m*sizeof(double));
  B = (double*) malloc(n*sizeof(double));
  resultado= (double*) malloc(m*sizeof(double));
  for(i = 0; i < m; i++){
    A[i] = (double*) malloc(n*sizeof(double));
    for(j = 0; j < n; j++){
      A[i][j] = 1;
    }
  }
  for(i = 0; i < m; i++) resultado[i] = 0;
  for(i = 0; i < n; i++) B[i] = 1;
  thread_count = strtol(argv[1],NULL,10);
  thread_handles = malloc(thread_count*sizeof(pthread_t));
  clock_gettime(CLOCK_MONOTONIC, &begin);
  for(thread = 0; thread < thread_count; thread++){
    pthread_create(&thread_handles[thread], NULL, pth_mat_vect, (void*) thread);}
  for(thread = 0; thread < thread_count; thread++){
    pthread_join(thread_handles[thread],NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
  printf("%lf\n", elapsed);
  free(thread_handles);
  return 0;
}