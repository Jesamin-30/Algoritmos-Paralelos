#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#include "pixel.h"

#define blur_size 3

__global__
void blur_kernel(pixel *in, pixel *out, unsigned w, unsigned h){
    unsigned col = threadIdx.x+(blockIdx.x*blockDim.x);
	unsigned row = threadIdx.y+(blockIdx.y*blockDim.y);
    
    if(col<w and row<h){
    	float valr = 0.0;
        float valg = 0.0;
        float valb = 0.0;
        float size = 0;

        int crow, ccol;
        for(int brow=-blur_size; brow<blur_size+1; brow++){
            for(int bcol=blur_size; bcol<blur_size+1; bcol++){
                crow = row+brow;
                ccol = col+bcol;

                if(crow>-1 and crow<h && ccol>-1 and ccol<w){
                    valr += in[(crow*w)+ccol].get_r();
                    valg += in[(crow*w)+ccol].get_g();
                    valb += in[(crow*w)+ccol].get_b();
                    size+=1;
                }
            }
        }

        out[(row*w)+col].set_r(valr/size);
        out[(row*w)+col].set_g(valg/size);
        out[(row*w)+col].set_b(valb/size);
    }
}

void kblur(pixel *h_in, pixel *h_gray, unsigned col, unsigned row){
    unsigned size = col*row*sizeof(pixel);

    pixel *d_in;
    pixel *d_out;

    cudaMalloc((void **)&d_in, size);
	cudaMalloc((void **)&d_out, size);

	cudaMemcpy(d_in, h_in, size, cudaMemcpyHostToDevice);

	unsigned block = 30;
	dim3 dimGrid(ceil(col/block), ceil(row/block), 1);
	dim3 dimBlock(block, block, 1);
	blur_kernel<<<dimGrid, dimBlock>>>(d_in, d_out, col, row);

	cudaMemcpy(h_gray, d_out, size, cudaMemcpyDeviceToHost);

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
	pixel *blur = new pixel[cols*rows];

	unsigned i, j; //tmp;
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
		    in[(i*cols)+j].set_r((float)src_in.at<cv::Vec3b>(i, j)[0]);
		    in[(i*cols)+j].set_g((float)src_in.at<cv::Vec3b>(i, j)[1]);
		    in[(i*cols)+j].set_b((float)src_in.at<cv::Vec3b>(i, j)[2]);
		}
	}	

	kblur(in, blur, cols, rows);

	cv::Mat src_out(rows, cols, CV_8UC3, cv::Scalar(0,0,0));
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
		    src_out.at<cv::Vec3b>(i, j)[0] = blur[(i*cols)+j].get_r();
		    src_out.at<cv::Vec3b>(i, j)[1] = blur[(i*cols)+j].get_g();
		    src_out.at<cv::Vec3b>(i, j)[2] = blur[(i*cols)+j].get_b();
		}
	}

	cv::namedWindow("window_blur", CV_WINDOW_NORMAL);
	cv::imshow("window_blur", src_out);

	cv::waitKey(0);
	cv::destroyWindow("window_in");
	cv::destroyWindow("window_blur");

	delete in;
	delete blur;
	return 0;
}