#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>

#define BLOCK_SIZE 		8
#define WRITE_TIME 		1
#define READ_TIME		1
#define NUM_ITERATIONS 	5
#define NUM_READERS 	5
#define INCREMENT 		100000

pthread_mutex_t mutex;
sem_t sem_writer;
long long *dataseg;
int read_count = 0;
long long data;

void *reader_thread(void *num);
void *writer_thread();

int main(int argc, char **argv)
{
	pthread_mutex_init(&mutex, NULL);
	sem_init(&sem_writer, 0, 1);

	dataseg = mmap(NULL, BLOCK_SIZE, 
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(dataseg == NULL)
	{
		perror("Could not get data segment");
		exit(1);
	}

	pthread_t read_tid[5];
	pthread_t write_tid;
	
	pthread_create(&write_tid, NULL, writer_thread, NULL);
	int i = 0, j = 0;
	while(i < NUM_READERS)
	{
		pthread_create(&read_tid[i], NULL, reader_thread, &i);
		i++;
	}

	while(j < NUM_READERS)
	{
		pthread_join(read_tid[j], NULL);
		j++;
	}
	pthread_join(write_tid, NULL);

	sem_close(&sem_writer);
	sem_destroy(&sem_writer);
	pthread_mutex_destroy(&mutex);

	return 0;
}

void *reader_thread(void *num)
{	
	int r_ind = *(int *)num;
	for(int i = 0; i < NUM_ITERATIONS; i++)
	{
		sleep(READ_TIME);
		pthread_mutex_lock(&mutex);
		read_count++;
		if(read_count == 1) 
		{
			sem_wait(&sem_writer);
		}
		pthread_mutex_unlock(&mutex);

		printf("%d %lld\n", r_ind, *dataseg);
		// *dataseg = 0;	// destroy the read value

		pthread_mutex_lock(&mutex);
		read_count--;
		if(read_count == 0)
		{
			sem_post(&sem_writer);
		}
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(0);
}

void *writer_thread()
{
	for(int i = 0; i < NUM_ITERATIONS; i++)
	{
		sleep(WRITE_TIME);
		sem_wait(&sem_writer);
		data += INCREMENT;
		*dataseg = data;
		sem_post(&sem_writer);
	}

	pthread_exit(0);
}