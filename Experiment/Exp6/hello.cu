#include <stdio.h>
__global__ void helloworld(void)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    printf("Hello World from (%d,%d)!\n", i , j);
}
int main()
{
    dim3 grid(2,4);
    dim3 block(8,16);
    helloworld <<<grid,block>>>();
    cudaDeviceSynchronize();
    return 0;
}
