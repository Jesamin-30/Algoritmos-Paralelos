#include <stdio.h>
#include <mpi.h>

int tam = 1000;

int sumatoria(int *vec, unsigned t){
	int contador = 0;
	for(unsigned i=0; i<t; i++){
		contador += vec[i];
	}	
	return contador;
}

int main(int argc, char const *argv[]){
	int my_rank, comm_sz;
	int vect[tam], particion, sp;
	int sum_local, sum_total;

	MPI_Init(NULL,NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	particion = tam/(comm_sz-1);

	if(my_rank == 0){
		for(int i=0; i<tam; i++)
			vect[i] = i;

		sp = 0;
		for(int i=1; i<comm_sz; i++){
			MPI_Send(&vect[sp], particion, MPI_INT, i, 0, MPI_COMM_WORLD);
			sp = sp + particion;
		}
	}
	else{			
		MPI_Recv(&vect[0], particion, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);			
	}

	sum_local = sumatoria(vect, particion);
	MPI_Reduce(&sum_local, &sum_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(my_rank == 0){
		printf("suma : %d\n", sum_total);
	}
	MPI_Finalize();

	return 0;
}