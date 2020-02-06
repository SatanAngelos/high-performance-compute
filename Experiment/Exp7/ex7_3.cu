//使用constant memory存放向量
//global memory
#include<stdio.h>
#include<math.h>
#include<time.h>
#include <stdlib.h>


int Max=16384;
int width=32;
double err = 0.1;

__constant__ double con_b[8192];

__global__ void multi(double *A,double *C,const int Max,int i){

	int idx=threadIdx.x+blockDim.x*blockIdx.x;
	//int idy=threadIdx.y+blockDim.y*blockIdx.y;
	if(idx<Max){
		int k=0;
		double sum=0;
		for(k=i*Max/2;k<(i+1)*Max/2;k++){
			sum+=A[idx*Max+k]*con_b[k%(Max/2)];
		}
		C[idx]+=sum;
	}
	
}

int main(){
	printf("使用constant memory存放向量:\n");
	double *A =(double *)malloc(Max * Max * sizeof(double));   //A	
	double b[Max];   //b
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
	
	double *A_d,*C_d;
	cudaMalloc((void **)&A_d,Max * Max * sizeof(double));
	
	cudaMalloc((void **)&C_d,Max *sizeof(double));
	
	clock_t start,end;
	start=clock();
	cudaMemcpy(A_d, A,Max*Max*sizeof(double),cudaMemcpyHostToDevice);
	//cudaMemcpyToSymbol(con_b, b, sizeof(double) * Max);
	cudaMemcpy(C_d, C,Max * sizeof(double), cudaMemcpyHostToDevice);
	
	dim3 block(width,1);
	dim3 grid(Max/block.x, 1);
	for(int i=0;i<2;i++){
		cudaMemcpyToSymbol(con_b, &b[i*Max/2], sizeof(double) * Max/2);
		multi<<<grid,block>>>(A_d,C_d,Max,i);
		
	}
	
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

        double a=test_c[i];
        double b=C[i];
        if (abs(a-b)>err)
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
	
	
	cudaFree(A_d);
	cudaFree(C_d);

	free(A);
	free(test_c);
	free(C);
	
	
}
