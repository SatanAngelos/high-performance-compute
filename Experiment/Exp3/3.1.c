//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include <semaphore.h>
#define size 10
sem_t sem;
int num;
void *count(void *thread_id){
	int customer_id = *((int *)thread_id);
	
	if(sem_wait(&sem)==0){
		usleep(100);
		printf("%d\n",num++);
		printf("customer %d receive service ...\n", customer_id);
		sem_post(&sem);
	}
	
	//cout<<num++<<endl;
	
}
int main(int argc, char *argv[]){
	//int sem_init(sem_t *sem, int pshared, unsigned int value);
	int i,j;
	sem_init(&sem,0,2);
	pthread_t id[size];	
	for(i=0;i<size;i++){
		int id_i=i;
		int ret=pthread_create(&id[i],NULL,count,&id_i);
		//printf("id %d arrived \n",i);
		usleep(100);
	}
	for(j=0;j<size;j++){
		pthread_join(id[j],NULL);
	}
	
	
	
	sem_destroy(&sem);
	return 0;
}

