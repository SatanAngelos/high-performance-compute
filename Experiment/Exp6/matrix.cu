#include <stdio.h>
#include <stdlib.h>
int Max = 8192;
int width = 32;

__global__ void Add(double *A, double *b, double *sum, const int Max)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if (i < Max && j < Max)
        sum[i * Max + j] = A[i * Max + j] + b[i * Max + j];
}

double CPU_time(double *a, double *b, double *c)
{
    clock_t start = clock();
    for (int i = 0; i < Max * Max; ++i)
        c[i] = a[i] + b[i];
    clock_t end = clock();
    return (end - start) * 1000 / CLOCKS_PER_SEC;
}

void Check_result(double *a, double *b, double *sum)
{
    bool flag = true;
    for (int i = 0; i < Max; ++i)
        for (int j = 0; j < Max; ++j)
        {
            if (a[i * Max + j] + b[i * Max + j] != sum[i * Max + j])
            {
                flag = false;
                printf("Fail : (%d,%d)\n", i, j);
                printf("Correct Answer :%lf , My Answer :%lf\n",
                       (i - 0.1 * j + 1) + (0.2 * j - 0.1 * i), sum[i * Max + j]);
            }
        }
    if (flag == true)
        printf("GPU result True\n");
}

int main()
{
	clock_t st,en;
    
    double *a_h = (double *)malloc(Max * Max * sizeof(double));
    double *b_h = (double *)malloc(Max * Max * sizeof(double));
    double *c_h = (double *)malloc(Max * Max * sizeof(double));
    for (int i = 0; i < Max; ++i)
        for (int j = 0; j < Max; ++j)
        {
            a_h[i * Max + j] = i - 0.1 * j + 1;
            b_h[i * Max + j] = 0.2 * j - 0.1 * i;
            c_h[i * Max + j] = 0;
        }

    
    double *A, *B, *C;
    cudaMalloc((void **)&A, Max * Max * sizeof(double));
    cudaMalloc((void **)&B, Max *Max * sizeof(double));
    cudaMalloc((void **)&C, Max * Max * sizeof(double));
	//start time
	st=clock();
	
    // Memcpy CPU -> GPU
    cudaMemcpy(A, a_h, Max * Max * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(B, b_h, Max * Max * sizeof(double), cudaMemcpyHostToDevice);

    
    dim3 block(width, width);
    dim3 grid(Max/block.x, Max/block.y);
    Add<<<grid, block>>>(A, B, C, Max);
	
   
    cudaMemcpy(c_h, C,Max * Max * sizeof(double), cudaMemcpyDeviceToHost);
	
	en=clock();
	float time1=(float)(en-st)*1000/CLOCKS_PER_SEC;
   
    
    Check_result(a_h, b_h, c_h);

    // print time of GPU,CPU

    printf("GPU Time:%lf ms \n", time1);
    printf("CPU Time:%lf ms \n", CPU_time(a_h, b_h, c_h));


    // free
   
    cudaFree(A);
    cudaFree(B);
    cudaFree(C);
    free(a_h);
    free(b_h);
    free(c_h);
}