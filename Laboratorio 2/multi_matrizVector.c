#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>



void mat_vect_mult(double local_A[], double local_x[], double local_y[], int local_m, int n, int local_n, MPI_Comm comm){
  double* x;
  int local_i,j;
  int local_ok=1;
  x = malloc(n*sizeof(double));
  MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, comm);
  for(local_i = 0; local_i < local_m; local_i++){
    local_y[local_i] = 0.0;
    for(j = 0; j <n; j++){
      local_y[local_i] += local_A[local_i*n+j] * x[j];
    }
  }
  free(x);
}


void read_vector(double local_a[], int local_n, int n, char vec_name[], int my_rank, MPI_Comm comm){
	double* a = NULL;
  	if(my_rank == 0){
	    a = malloc(n*sizeof(double));
	    printf("EL VECTOOOR: %s\n", vec_name);
	    for(int i = 0; i < n; i++){
	    	scanf("%lf", &a[i]);
	    }
	    MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE,0,comm);
	    free(a);
  	}
    else{
      MPI_Scatter(a,local_n,MPI_DOUBLE,local_a,local_n,MPI_DOUBLE,0,comm);
    }
}

void print_vector(double local_b[], int local_n,int n, char title[],int my_rank,MPI_Comm comm){
	double *b=NULL;
	if(my_rank==0){
		b=malloc(n*sizeof(double));
		MPI_Gather(local_b,local_n,MPI_DOUBLE,b,local_n,MPI_DOUBLE,0,comm);//recoge todos los datos
		printf("%s\n",title);
		for(int i=0 ; i< n;i++){
			printf("%f",b[i]);
		}
		printf("\n");
		free(b);
	}
	else{
		MPI_Gather(local_b,local_n,MPI_DOUBLE,b,local_n,MPI_DOUBLE,0,comm);
	}
}

int main(int argc, char const *argv[])
{
	int comm_sz,my_rank;
	int m=4,n=4, local_n,local_m;
	MPI_Comm comm;
	double start,finish;

	MPI_Init(NULL,NULL);
	comm = MPI_COMM_WORLD;//comunicador con otros procesos
   	MPI_Comm_size(comm, &comm_sz);//ingreso por terminal
   	MPI_Comm_rank(comm, &my_rank);//id del procesador

   	local_n=n/comm_sz;
   	local_m=m/comm_sz;
   	double A[local_n*local_m],B[local_n],resultado[local_m];
   	read_vector(A,local_n*local_m,n*m,"A",my_rank,comm);
   	read_vector(B,local_n,n,"B",my_rank,comm);

   	start=MPI_Wtime();
   	mat_vect_mult(A,B,resultado,local_m,n,local_n,comm);
   	finish=MPI_Wtime();

   	print_vector(resultado,local_m,m,"RESULTADO ",my_rank,comm);
   	printf("procesador %d tiempo: %e seconds \n",my_rank,finish-start);
   	MPI_Finalize();
	return 0;
}