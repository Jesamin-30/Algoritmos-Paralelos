#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "pixel.h"

__global__
void gray_scale_kernel(pixel *in, float *gray, unsigned col, unsigned row){
    unsigned c = threadIdx.x+(blockIdx.x*blockDim.x);
	unsigned r = threadIdx.y+(blockIdx.y*blockDim.y);
    
    if(c<col and r<row){    	
    	unsigned id = (r*col)+c;
    	gray[id] = (0.299*in[id].get_r())+
                    (0.587*in[id].get_g())+
                    (0.114*in[id].get_b());
    }
}

void gray_scale(pixel *in, float *h_out, unsigned col, unsigned row){
    unsigned msize = col*row*sizeof(pixel);     // pixel vector
    unsigned rsize = col*row*sizeof(float);     // float vector

    pixel *d_in;
    float *d_out;

    cudaMalloc((void **)&d_in, msize);
	cudaMalloc((void **)&d_out, rsize);

	cudaMemcpy(d_in, in, msize, cudaMemcpyHostToDevice);

	unsigned block = 16;
	dim3 dimGrid(ceil(col/block), ceil(row/block), 1);
	dim3 dimBlock(block, block, 1);
	gray_scale_kernel<<<dimGrid, dimBlock>>>(d_in, d_out, col, row);

	cudaMemcpy(h_out, d_out, rsize, cudaMemcpyDeviceToHost);

	cudaFree(d_in);
	cudaFree(d_out);
}


int main(int argc, char const *argv[]){

	cv::Mat src_in = cv::imread("lena.png", cv::IMREAD_COLOR);
	if(src_in.empty()){
		std::cout << "Error : Image cannot be loaded..!!n";
		return 0;
	}

	cv::namedWindow("window_in", CV_WINDOW_NORMAL);
	cv::imshow("window_in", src_in);

	unsigned cols = src_in.cols;
	unsigned rows = src_in.rows;

	std::cout << "cols: " << cols << "\n";
	std::cout << "rows: " << rows << "\n";

	pixel *in = new pixel[cols*rows];
	float *out = new float[cols*rows];


	unsigned tmp, i, j;
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
		    in[(i*cols)+j].set_r((float)src_in.at<cv::Vec3b>(i, j)[0]);
		    in[(i*cols)+j].set_g((float)src_in.at<cv::Vec3b>(i, j)[1]);
		    in[(i*cols)+j].set_b((float)src_in.at<cv::Vec3b>(i, j)[2]);
		}
	}

	gray_scale(in, out, cols, rows);

	cv::Mat src_out(rows,cols, CV_8UC3, cv::Scalar(0,0,0));	
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
			tmp = (i*cols)+j;
			src_out.at<cv::Vec3b>(i, j)[0] = out[tmp];
			src_out.at<cv::Vec3b>(i, j)[1] = out[tmp];
			src_out.at<cv::Vec3b>(i, j)[2] = out[tmp];
		}
	}

	cv::namedWindow("window_out", CV_WINDOW_NORMAL);
	cv::imshow("window_out", src_out);

	cv::waitKey(0);
	cv::destroyWindow("window_in");
	cv::destroyWindow("window_out");

	delete in;
	delete out;
	return 0;
}