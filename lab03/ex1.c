#include <pthread.h>
#include <stdio.h>
void* thread1();
void* thread2();

int main(){
	//Create the pthread pointer types
	pthread_t i1, i2;
	
	//Create the threads, each with their specific function pointer
	pthread_create(&i1, NULL, thread1, NULL);
	pthread_create(&i2, NULL, thread2, NULL);
	
	//Wait until threads have finished
	pthread_join(i1, NULL);
	pthread_join(i2, NULL);
	
	//Print
	printf("Hello, I am the main process\n");
}

void *thread1(){
	//sleep(5);
	printf("Hello, I am thread 1\n");
}

void *thread2(){
	//sleep(5);
	printf("Hello, I am thread 2\n");
}