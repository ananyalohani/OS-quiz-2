#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#define CHAR_BLOCK_SIZE 500
#define INT_BLOCK_SIZE sizeof(int)
#define CHAR_KEY 1234
#define INT_KEY 2456
#define SEM_WRITER "/writer"
#define SEM_READER "/reader"

void *attach_memblock(int size, key_t key);
bool detach_memblock(void *block);
bool destroy_memblock(key_t key);

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr, "Pass two arguments.\n");
		return -1;
	}

	sem_unlink(SEM_WRITER);
	sem_unlink(SEM_READER);

	pid_t pid = fork();
	if(pid == 0)
	{
		sem_t *sem_writer = sem_open(SEM_WRITER, 0);
		if(sem_writer == SEM_FAILED)
		{
			perror("sem_open/writer");
			exit(1);
		}

		sem_t *sem_reader = sem_open(SEM_READER, 0);
		if(sem_reader == SEM_FAILED)
		{
			perror("sem_open/reader");
			exit(1);
		}

		char *charblock = (char *)attach_memblock(CHAR_BLOCK_SIZE, CHAR_KEY);
		int *intblock = (int *)attach_memblock(INT_BLOCK_SIZE, INT_KEY);
		if(charblock == NULL || intblock == NULL)
		{
			perror("Block creation");
			exit(1);
		}

		sem_wait(sem_reader);
		strncpy(charblock, argv[1], CHAR_BLOCK_SIZE);
		*intblock = atoi(argv[2]);
		sem_post(sem_writer);

		sem_close(sem_writer);
		sem_close(sem_reader);
		detach_memblock(charblock);
		detach_memblock(intblock);
		
		exit(0);
	}
	else if(pid > 0)
	{
		sem_t *sem_writer = sem_open(SEM_WRITER, O_CREAT, 0660, 0);
		if(sem_writer == SEM_FAILED)
		{
			perror("sem_open/writer");
			exit(1);
		}

		sem_t *sem_reader = sem_open(SEM_READER, O_CREAT, 0660, 1);
		if(sem_reader == SEM_FAILED)
		{
			perror("sem_open/reader");
			exit(1);
		}

		// wait(NULL);
		char *charblock = (char *)attach_memblock(CHAR_BLOCK_SIZE, CHAR_KEY);
		int *intblock = (int *)attach_memblock(INT_BLOCK_SIZE, INT_KEY);
		if(charblock == NULL || intblock == NULL)
		{
			perror("Couldn't get block");
			exit(1);
		}

		sem_wait(sem_writer);
		printf("%s\n", charblock);
		printf("%d\n", *intblock);
		sem_post(sem_reader);

		sem_close(sem_writer);
		sem_close(sem_reader);
		detach_memblock(charblock);
		detach_memblock(intblock);
		
	}

	if(!destroy_memblock(CHAR_KEY) || !destroy_memblock(INT_KEY))
	{
		perror("Destroying block");
	}
	
	return 0;
}

void *attach_memblock(int size, key_t key)
{
	int block_id = shmget(key, size, 0644 | IPC_CREAT);
	if(block_id == -1) return NULL;

	return shmat(block_id, NULL, 0);
}

bool detach_memblock(void *block)
{
	if(shmdt(block) == -1) return false;
	return true;
}

bool destroy_memblock(key_t key)
{
	int block_id = shmget(key, 0, 0644 | IPC_CREAT);
	if(block_id == -1) return NULL;

	return (shmctl(block_id, IPC_RMID, NULL) != -1);
}