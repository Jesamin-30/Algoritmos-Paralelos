#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#define tile_width 32

__global__
void normal_square_matrix_mult_kernel(int *m, int *n, int *p, unsigned width){
	unsigned col = threadIdx.x+(blockIdx.x*blockDim.x);
	unsigned row = threadIdx.y+(blockIdx.y*blockDim.y);

	if(col<width and row<width){
		int pvalue = 0;
		for(unsigned k=0; k<width; k++){
			pvalue += m[(row*width)+k]*n[(k*width)+col];
		}
		p[(row*width)+col] = pvalue;
	}
}

__global__
void tile_square_matrix_mult_kernel(int *m, int *n, int *p, unsigned width){
	__shared__ int mds[tile_width][tile_width];
	__shared__ int nds[tile_width][tile_width];

	unsigned bx = blockIdx.x;
	unsigned by = blockIdx.y;

	unsigned tx = threadIdx.x;
	unsigned ty = threadIdx.y;

	unsigned row = (by*tile_width)+ty;	
	unsigned col = (bx*tile_width)+tx;

	int pvalue = 0;
	unsigned ph, k;
	for(ph=0; ph<width/tile_width; ph++){
		mds[ty][tx] = m[(row*width)+(ph*tile_width) + tx];
		nds[ty][tx] = n[(((ph*tile_width)+ty)*width) + col];
		__syncthreads();

		for(k=0; k<tile_width; k++){
			pvalue += mds[ty][k]*nds[k][tx];
		}
		__syncthreads();
	}

	p[(row*width)+col] = pvalue;
}

void square_matrix_mult(int *m, int *n, int *p, unsigned width, unsigned block, char type){
	unsigned size = width*width*sizeof(int);
	int *d_m, *d_n, *d_p;

	cudaMalloc((void **)&d_m, size);
	cudaMalloc((void **)&d_n, size);
	cudaMalloc((void **)&d_p, size);

	cudaMemcpy(d_m, m, size, cudaMemcpyHostToDevice);	
	cudaMemcpy(d_n, n, size, cudaMemcpyHostToDevice);	

	dim3 dimGrid(ceil(width/block), ceil(width/block), 1);
	dim3 dimBlock(block, block, 1);

	switch(type){
		case 'n':
			normal_square_matrix_mult_kernel<<<dimGrid, dimBlock>>>(d_m, d_n, d_p, width);
			break;
		case 't':
			tile_square_matrix_mult_kernel<<<dimGrid, dimBlock>>>(d_m, d_n, d_p, width);
			break;
		default:
			break;
	}	

	cudaMemcpy(p, d_p, size, cudaMemcpyDeviceToHost);

	cudaFree(d_m);
	cudaFree(d_n);
	cudaFree(d_p);
}


int main(int argc, char const *argv[]){	
	unsigned width = 1024;
	unsigned block = 32;

	unsigned ntotal = width*width;	

	int *h_m = new int[ntotal];
	int *h_n = new int[ntotal];
	int *h_p = new int[ntotal];
	int *h_r = new int[ntotal];

	unsigned i, j;
	for(i=0; i<width; i++){
		for(j=0; j<width; j++){
		    h_m[(i*width)+j] = j;
		    h_n[(i*width)+j] = j;
		    h_p[(i*width)+j] = 0;
		    h_r[(i*width)+j] = 0;
		}
	}
		
	struct timeval ti, tf;
	double time;

	gettimeofday(&ti, NULL);

	square_matrix_mult(h_m, h_n, h_p, width, block, 'n');

	gettimeofday(&tf, NULL);
	
	time = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000;
	printf("[%ix%i] memoria global: %.8lf s\n", width, width, time/1000);

	gettimeofday(&ti, NULL);
		square_matrix_mult(h_m, h_n, h_r, width, block, 't');
	gettimeofday(&tf, NULL);
	time = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000;
	printf("[%ix%i] memoria compartida: %.8lf s\n", width, width, time/1000);

	delete h_m;
	delete h_n;
	delete h_p;
	delete h_r;

	return 0;
}
