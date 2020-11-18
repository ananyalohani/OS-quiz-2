#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// sem_t sem_reader;
pthread_mutex_t mutex;
sem_t sem_writer;
int read_count = 0;
long data = 10000;
int ind = 1;

void *reader_thread();
void *writer_thread();

int main(int argc, char **argv)
{
	// sem_init(sem_reader, 0, 1);
	pthread_mutex_init(&mutex, NULL);
	sem_init(&sem_writer, 0, 1);

	pthread_t read_tid[5];
	pthread_t write_tid[2];

	// for(int i = 1; i <= 5; i++)
	// {
	// 	printf("i=%d", i);
	// 	pthread_create(&read_tid[i], NULL, reader_thread, NULL);
	// }

	// for(int j = 1; j <= 2; j++)
	// {
	// 	printf("i=%d", i);
	// 	pthread_create(&write_tid[j], NULL, writer_thread, NULL);
	// }

	for(int i = 1; i <= 10; i++)
	{
		int remtwo = i%2;
		int remfive = i%5;

		if(remtwo == 0)
		{
			pthread_create(&read_tid[i/2], NULL, reader_thread, NULL);
		}
		if(remfive == 0)
		{
			pthread_create(&write_tid[i/5], NULL, writer_thread, NULL);
		}
		if(remtwo == 0)
		{
			pthread_join(read_tid[i/2], NULL);
		}
		if(remfive == 0)
		{
			pthread_join(write_tid[i/5], NULL);
		}
	}

	sem_close(&sem_writer);
	sem_destroy(&sem_writer);
	pthread_mutex_destroy(&mutex);

	return 0;
}

void *reader_thread()
{	
	pthread_mutex_lock(&mutex);
	read_count++;
	if(read_count == 1) 
	{
		sem_wait(&sem_writer);
	}
	pthread_mutex_unlock(&mutex);

	printf("%d %ld\n", ind++, data);

	pthread_mutex_lock(&mutex);
	read_count--;
	if(read_count == 0)
	{
		sem_post(&sem_writer);
	}
	pthread_mutex_unlock(&mutex);

	pthread_exit(0);
}

void *writer_thread()
{
	sem_wait(&sem_writer);
	data = data * 4;
	sem_post(&sem_writer);

	pthread_exit(0);
}