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
	int temporal, total;

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

		temporal = sumatoria(vect, particion);
        total = temporal;
        for (int i = 1; i < comm_sz; ++i)
        {
            MPI_Recv(&temporal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += temporal;
        }
        printf("suma : %d\n", total);
	}


	else{			
		MPI_Recv(&vect[0], particion, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);			
	}

	temporal = sumatoria(vect, particion);
	MPI_Send(&temporal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);  

	MPI_Finalize();

	return 0;
}