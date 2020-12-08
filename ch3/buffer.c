#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10
#define THREAD_COUNT 4 

int buffer[BUFFER_SIZE]; 
int in = -1; // product
int out = -1; // consumer
int num = 0; // 최종 생산

sem_t buffer_slot; // 빈 슬롯인 세마포어 변수 생성
sem_t items; // item 유/무
sem_t mutex; // 이진 세마포어

void *product(void *arg);
void *consumer(void *arg); 

int main(void){ 
	sem_init(&buffer_slot, 0, BUFFER_SIZE); 
	sem_init(&items, 0, 0); 
	sem_init(&mutex, 0, 1);

	pthread_t threads[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; i++){
		if (i % 2 == 0)
			pthread_create(&threads[i], NULL, product, NULL); //product
		else
			pthread_create(&threads[i], NULL, consumer, NULL); //consumer
	}
	for (int i = 0; i < THREAD_COUNT; i++){	pthread_join(threads[i], NULL);}
	printf("\n result : %d \n \n", num);
	pthread_exit(NULL); // 쓰레드 종료
	sem_destroy(&buffer_slot); // 세마포어 삭제
	sem_destroy(&items); // 세마포어 삭제
	return 0;
}

void *product(void *arg){
	int data;

	for (int i = 0; i < BUFFER_SIZE * 10; i++){
		printf("produce : %d \n", i);
		sem_wait(&buffer_slot);
		sem_wait(&mutex); //start
		num++;
		in++;
		in %= BUFFER_SIZE;
		buffer[in] = i;
		sem_post(&mutex); //end
		sem_post(&items);
	}
}


void *consumer(void *arg){ 
	int data;
	for (int i = 0; i < BUFFER_SIZE * 10; i++){
		sem_wait(&items);
		sem_wait(&mutex); // start
		out++;
		num--;
		out %= BUFFER_SIZE;
		data = buffer[out];
		sem_post(&mutex); // end
		sem_post(&buffer_slot);
		printf("consume : %d \n", data);
	}
}
