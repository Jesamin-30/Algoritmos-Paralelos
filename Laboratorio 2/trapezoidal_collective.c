#include <stdio.h>
#include <string.h>
#include <mpi.h>


double funcion(double x)
{
    return x*x;
}

double trapezoidal(double a, double b, int n, double h){
    double estimate, x;
    estimate = (funcion(a)+funcion(b))/2;

    for(int i=1; i<n; i++){
        x = a+(i*h);
        estimate += funcion(x);
    }

    estimate = estimate*h;
    return estimate;
}


int main()
{
    int my_rank, comm_sz, n, local_n;
    n=1000;
    double a,b,h,local_a, local_b;
    a=0;
    b=2;
    double local_int, total_int;
    int source;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    h = (b-a)/n;
    local_n = n/comm_sz;

    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
    local_int = trapezoidal(local_a, local_b, local_n, h);

    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (my_rank == 0) {
        printf("Trapezoidal: %.12lf\n", total_int);
    }
    MPI_Finalize();
    return 0;
}