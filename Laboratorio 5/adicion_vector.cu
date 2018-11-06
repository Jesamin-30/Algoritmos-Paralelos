#include <iostream>
#include <stdlib.h>
#include <stdio.h>

__global__//host function to generate a grid of threads on a device.
void vect_add_kernel(int *a, int *b, int *c, int n){
	unsigned i = (blockDim.x*blockIdx.x)+threadIdx.x;

	if(i<n)	c[i] = a[i]+b[i];
}

void vectAdd(int *a, int *b, int *c, unsigned n, unsigned thc){
	unsigned size = n*sizeof(int);
	int *d_a, *d_b, *d_c;

	cudaMalloc((void **)&d_a, size);
	cudaMalloc((void **)&d_b, size);
	cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);	
	cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

	cudaMalloc((void **)&d_c, size);
	vect_add_kernel<<<(ceil(n/thc), thc)>>>(d_a, d_b, d_c, n);

	cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_c);
}

void print(int *v, unsigned s){
	for(unsigned i=0; i<t; i++)	std::cout << v[i] << " ";
	std::cout << "\n";
}

int int main(int argc, char const *argv[]){
	unsigned n = 1000;
	unsigned thc = 256;

	int *h_a = new int[n];
	int *h_b = new int[n];
	int *h_c = new int[n];

	unsigned i;
	for(i=0; i<n; i++){
		h_a[i] = i;
		h_b[i] = i;
	}

	vectAdd(h_a, h_b, h_c, n, thc);

	print(h_c, n)

	delete h_a;
	delete h_b;
	delete h_c;

	return 0;
}