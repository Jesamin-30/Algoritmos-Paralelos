#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void Hello(void) {
	int my_rank = omp_get_thread_num();//Devuelve el número de hilo del que se ejecuta
	int thread_count = omp_get_num_threads();//number of threads used in a parallel region

	printf("Hello from thread %d of %d\n", my_rank, thread_count);
}


int main(int argc, char* argv[]) {
	int thread_count = strtol(argv[1], NULL, 10); //Get number of threads from command line 
	#pragma omp parallel num_threads(thread_count)
		Hello();
	return 0;
}

// gcc -fopenmp hello.c
// ./a.out 4
