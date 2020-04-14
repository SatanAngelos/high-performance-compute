// 使用global memory合并访存


#include<stdio.h>
#include<math.h>
#include<time.h>
#include <stdlib.h>
#define TILE_WIDTH 32

__global__ void multi(int m, int n, int k, double *A, double *B, double *C)
{
    int idx = blockIdx.y * blockDim.y + threadIdx.y;
    int idy	= blockIdx.x * blockDim.x + threadIdx.x;

    if ((idx < m) && (idy < k))
    {
        double Cvalue = 0.0;
        for (int i = 0; i < n; ++i)
            Cvalue += A[idx * n + i] * B[idy + i * k];
        C[idx * k + idy] = Cvalue;
    }
}


int main()
{
 
	printf("global memory:\n");
    int m = 4096, n = 4096, k = 4096;
    double *A = (double *)malloc(m * n * sizeof(double));
    double *B = (double *)malloc(n * k * sizeof(double));
    double *C = (double *)malloc(m * k * sizeof(double));
    double *test_c = (double *)malloc(m * k * sizeof(double));
	int i,j;
    for (i = 0; i < m; ++i)
        for (j = 0; j < m; ++j)
        {
            A[i * m + j] = (i - 0.1 * j + 1) / (i + j + 1);
            B[i * m + j] = (j - 0.2 * i + 1) * (i + j + 1) / (i * i + j * j + 1);
            C[i * m + j] = 0.0;
        }


    int size = sizeof(double);
 
	double *A_d,*b_d,*C_d;

    cudaMalloc((void **)&A_d, m * n * size);
    cudaMalloc((void **)&b_d, n * k * size);
    cudaMalloc((void **)&C_d, m * k * size);
	
	clock_t start,end;
	start=clock();
    
    cudaMemcpy(A_d, A, size * m * n, cudaMemcpyHostToDevice);
    cudaMemcpy(b_d, B, size * n * k, cudaMemcpyHostToDevice);
    cudaMemcpy(C_d, C, size * m * k, cudaMemcpyHostToDevice);

 
    dim3 dimGrid((k - 1) / TILE_WIDTH + 1, (m - 1) / TILE_WIDTH + 1, 1);
    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH, 1);

   
    multi<<<dimGrid, dimBlock>>>(m, n, k, A_d, b_d, C_d);

    cudaMemcpy(C, C_d, size * m * k, cudaMemcpyDeviceToHost);

    end=clock();
	double time=(end-start)*1000/CLOCKS_PER_SEC;
	
	//cpu:
    clock_t start_c,end_c;
    start_c=clock();
	
    for (i = 0; i < m; ++i){

        for (j = 0; j < m; ++j)
         {
			double sum=0;
			for(int k=0;k<m;k++){
			sum+=A[i*m+k]*B[k*m+j];
			}
			test_c[i * m + j] = sum;
         }
    }
    end_c=clock();
	double time_C=(end_c-start_c)*1000/CLOCKS_PER_SEC;
	printf("GPU TIME:%lf ms\n",time);
	printf("CPU TIME:%lf ms\n",time_C);

    //比较结果
    bool flag = true;
    for (int i = 0; i < 10; ++i)
    {
        if (abs(test_c[i] - C[i]) > 0.001)
        {
            flag = false;
			//printf("cpu:%lf gpu:%lf\n",test_c[i],C[i]);
            
        }
    }
    if (flag)
		printf("Result Correct!\n");
    else
		printf("Result Error!\n");
        

    cudaFree(A_d);
    cudaFree(b_d);
    cudaFree(C_d);
    free(A);
    free(B);
    free(C);
    free(test_c);
    return 0;
}