//合并 访存
#include<stdio.h>
#include<math.h>
#include<time.h>
#include <stdlib.h>

int Max=16384;
int width=32;

typedef struct  {
    double A1;
    double A2;
    double A3;
    double A4;
}stru;

__global__ void multi(stru *A,stru *b,double *C,const int Max){
	
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int idy = blockIdx.y * blockDim.y + threadIdx.y;
	
    double sum=0.0;
    if(idx<Max && idy<Max && idx==idy){
        for(int i=0;i<Max/4;i++){

            sum+=A[idx*Max/4+i].A1*b[i*4].A1+A[idx*Max/4+i].A2*b[i*4+1].A1+A[idx*Max/4+i].A3*b[i*4+2].A1+A[idx*Max/4+i].A4*b[i*4+3].A1;
			
        }
        C[idx]=sum;
    }
	
}

int main(){
	printf("合并访存:\n");
	stru *A =(stru *)malloc(Max * Max/4 * sizeof(stru));   //A	
	stru *b =(stru *)malloc(Max * sizeof(stru));   //b
	double *C =(double *)malloc(Max * sizeof(double));   //C
	double *test_c=(double *)malloc(Max * sizeof(double));  //cpu_test
	
	int i,j;
	for(i=0;i<Max;i++){
		for(j=0;j<Max/4;j++){
			A[i*Max/4+j].A1=i-0.1*j*4+1;
			A[i*Max/4+j].A2=i-(0.1*j*4+1)+1;
			A[i*Max/4+j].A3=i-(0.1*j*4+2)+1;
			A[i*Max/4+j].A4=i-(0.1*j*4+3)+1;
		}
	}
	for(i=0;i<Max;i++){
		b[i].A1=log(sqrt(i*i-i+2));
        b[i].A2=0.0;
        b[i].A3=0.0;
        b[i].A4=0.0;
		C[i]=0.0;
	}
	
	stru *A_d,*b_d;
	double *C_d;
	cudaMalloc((void **)&A_d,Max * Max/4 * sizeof(stru));
	cudaMalloc((void **)&b_d,Max *sizeof(stru));
	cudaMalloc((void **)&C_d,Max *sizeof(double));
	
	clock_t start,end;
	start=clock();
	cudaMemcpy(A_d, A,Max*Max/4*sizeof(stru),cudaMemcpyHostToDevice);
	cudaMemcpy(b_d, b,Max*sizeof(stru),cudaMemcpyHostToDevice);
	cudaMemcpy(C_d, C,Max * sizeof(double), cudaMemcpyHostToDevice);
	
	dim3 block(width,width);
	dim3 grid(Max/block.x, Max/block.y);
	multi<<<grid,block>>>(A_d,b_d,C_d,Max);
	
	cudaMemcpy(C, C_d, Max * sizeof(double), cudaMemcpyDeviceToHost);

	end=clock();
	double time=(end-start)*1000/CLOCKS_PER_SEC;
	
	//check result:
	
    
	//cpu:
    clock_t start_c,end_c;
    start_c=clock();
	
    for (int i = 0; i < Max; ++i){

        for (int j = 0; j < Max/4; ++j)
         {
             test_c[i]+=A[i*Max/4+j].A1*b[j*4].A1+A[i*Max/4+j].A2*b[j*4+1].A1+A[i*Max/4+j].A3*b[j*4+2].A1+A[i*Max/4+j].A4*b[j*4+3].A1;
         }
    }
    end_c=clock();
	bool flag = true;
    for (int i = 0; i < Max; ++i){

        float a=test_c[i];
        float b=C[i];
        if (a!=b)
        {
			printf("cpu:%lf gpu:%lf\n",a,b);
            flag = false;
        }

    }
    if (flag == true)
        printf("result correct\n");
    else{
        printf("resul wrong\n");
    }
	
	double time_C=(end_c-start_c)*1000/CLOCKS_PER_SEC;
	printf("GPU TIME:%lf ms\n",time);
	printf("CPU TIME:%lf ms\n",time_C);
	
	cudaFree(A_d);
	cudaFree(b_d);
	cudaFree(C_d);

	free(A);
	free(b);
	free(C);
	
	
}
