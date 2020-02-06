//global memory
#include<stdio.h>
#include<math.h>
#include<time.h>
#include <stdlib.h>

int Max=16384;
int width=32;

__global__ void multi(double *A,double *b,double *C,const int Max){
	int idx=threadIdx.x+blockDim.x*blockIdx.x;
	int idy=threadIdx.y+blockDim.y*blockIdx.y;
	if(idx<Max && idy<Max && idx==idy){
		int k=0;
		double sum=0;
		for(k=0;k<Max;k++){
			sum+=A[idx*Max+k]*b[k];
		}
		C[idx]=sum;
	}
	
}

int main(){
	printf("global memory:\n");
	double *A =(double *)malloc(Max * Max * sizeof(double));   //A	
	double *b =(double *)malloc(Max * sizeof(double));   //b
	double *C =(double *)malloc(Max * sizeof(double));   //C
	double *test_c=(double *)malloc(Max * sizeof(double));  //cpu_test
	
	int i,j;
	for(i=0;i<Max;i++){
		for(j=0;j<Max;j++){
			A[i*Max+j]=i-0.1*j+1;
		}
	}
	for(i=0;i<Max;i++){
		b[i]=log(sqrt(i*i-i+2));
		C[i]=0.0;
	}
	
	double *A_d,*b_d,*C_d;
	cudaMalloc((void **)&A_d,Max * Max * sizeof(double));
	cudaMalloc((void **)&b_d,Max *sizeof(double));
	cudaMalloc((void **)&C_d,Max *sizeof(double));
	
	clock_t start,end;
	start=clock();
	cudaMemcpy(A_d, A,Max*Max*sizeof(double),cudaMemcpyHostToDevice);
	cudaMemcpy(b_d, b,Max*sizeof(double),cudaMemcpyHostToDevice);
	cudaMemcpy(C_d, C,Max * sizeof(double), cudaMemcpyHostToDevice);
	
	dim3 block(width,width);
	dim3 grid(Max/block.x, Max/block.y);
	multi<<<grid,block>>>(A_d,b_d,C_d,Max);
	
	cudaMemcpy(C, C_d, Max * sizeof(double), cudaMemcpyDeviceToHost);

	end=clock();
	double time=(end-start)*1000/CLOCKS_PER_SEC;
	
	//cpu:
    clock_t start_c,end_c;
    start_c=clock();
	
    for (int i = 0; i < Max; ++i){

        for (int j = 0; j < Max; ++j)
         {
            test_c[i]+=A[i*Max+j]*b[j];
         }
    }
    end_c=clock();
	double time_C=(end_c-start_c)*1000/CLOCKS_PER_SEC;
	printf("GPU TIME:%lf ms\n",time);
	printf("CPU TIME:%lf ms\n",time_C);
	
	//check result:
	
    bool flag = true;
    for (int i = 0; i < Max; ++i){

        float a=test_c[i];
        float b=C[i];
        if (a!=b)
        {
            flag = false;
        }

    }
    if (flag == true)
        printf("result correct\n");
    else{
        printf("resul wrong\n");
    }
	
	cudaFree(A_d);
	cudaFree(b_d);
	cudaFree(C_d);

	free(A);
	free(b);
	free(C);
	
	
}
