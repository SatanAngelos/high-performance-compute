#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include<time.h>
#include <semaphore.h>

#define max 128
#define size 128

int counter;
sem_t count_sem;
sem_t barrier_sem;
int thread_count;


double **C=NULL;
double **A=NULL;
double **B=NULL;

struct node {
	int x;
	int y;
	long r;
};

typedef struct  p{
	int r;
	int i;
	int j;
	int dm;
	
	double local_sum;
}p;
p *ptr = NULL;

void barrier() {
	int j;
	sem_wait(&count_sem);
	if (counter == thread_count - 1) {
		counter = 0;
		sem_post(&count_sem);
		for (j = 0; j < thread_count - 1; j++)
			sem_post(&barrier_sem);
	}
	else {
		counter++;
		sem_post(&count_sem);
		sem_wait(&barrier_sem);
	}
}


void* compute(void* tmp){  //矩阵相乘 
	int k;
	p *flag = (p*)tmp;
	int r = flag->r;
	int i = flag->i;
	int j = flag->j;
	int dm = flag->dm;
	double local_sum = flag->local_sum;
	
	for (k = 0; k < dm; k++)
		local_sum += A[i][r*dm+k] * B[r*dm+k][j];
	flag->local_sum = local_sum;
	return NULL;
	
}

void matrix(){  //生成矩阵 
	int i,j;
	//malloc
	A = (double**)malloc(size * sizeof(double*));
	B = (double**)malloc(size * sizeof(double*));
	C = (double**)malloc(size * sizeof(double*));
	for (i = 0; i < size; i++) {
		A[i] = (double*)malloc(size * sizeof(double));
		B[i] = (double*)malloc(size * sizeof(double));
		C[i] = (double*)malloc(size * sizeof(double));
	}
	
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			A[i][j] = (i-0.1*j+1)/(i+j+1);
			B[i][j] = (j-0.2*i+1)*(i+j+1)/(i*i+j*j+1);
			C[i][j] = 0;
		}
	}

}
int main(int argc,char*argv[]){
	int i,j,k;
	long thread;
	thread_count=strtol(argv[1],NULL,10);
	pthread_t* thread_handles; 
	int d=size/thread_count;
	clock_t start, end;
	//生成矩阵 
	matrix();
	
	clock_t s_start , s_finish;
	s_start = clock();	
	for(i=0; i<size; i++)
		for(j=0; j<size; j++)
			C[i][j] = 0;
	for(i=0; i<size; i++)
		for(j=0; j<size; j++)
			for(k=0; k<size; k++)
				C[i][j] += A[i][k] * B[k][j];
	s_finish = clock();
	double s_time = (double)(s_finish - s_start);
	printf("Serial method cost time: %f ms\n" , s_time);
	
	thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	ptr = (p*)malloc(thread_count * sizeof(p));
	start = clock();
	
	
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			for(thread=0;thread<thread_count;thread++){
				ptr[thread].i=i;
				ptr[thread].j=j;
				ptr[thread].dm=d;
				ptr[thread].r=thread;
				ptr[thread].local_sum=0;
				pthread_create(&thread_handles[thread],NULL,compute,(void*)(&ptr[thread]));
			}
			for(thread=0;thread<thread_count;thread++){
				pthread_join(thread_handles[thread],NULL);
			}
			for(thread=0;thread<thread_count;thread++){
				C[i][j]+=ptr[thread].local_sum;
			}
		}	
	}
	end = clock();
	double p_time=(double)(end-start)/CLOCKS_PER_SEC;
	
	printf("Parallel method cost time: %f ms\n:  %lf s\n\n", p_time);
	double speedup = s_time / p_time;
	printf("The speedup is %f \n" , speedup);
	
	for (i = 0; i < size; i++) {
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}
	free(A);
	free(B);
	free(C);	
	return 0;
}



