#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define THINK -1
#define EAT 0
#define HUNGRY 1
#define LEFT (pnum + 4) % N
#define RIGHT (pnum + 1) % N

struct philos 
{
    int state;
    int num;
    int eaten;
};

sem_t mutex;
sem_t chop_lock[N];
struct philos phil[N];

void test(int pnum);
void pick_up_chop(int pnum);
void put_down_chop(int pnum);
void *phil_thread(void *num);
int have_all_eaten();

int main(int argc, char **argv)
{
    sem_init(&mutex, 0, 1);
    pthread_t phil_tid[N];

    for(int i = 0; i < N; i++)
    {
        sem_init(&chop_lock[i], 0, 0);
        phil[i].state = THINK;
        phil[i].num = i; 
        phil[i].eaten = 0;
    }
    for(int i = 0; i < N; i++)
    {
        pthread_create(&phil_tid[i], NULL, phil_thread, &phil[i].num);
        // printf("P%d is thinking\n", i + 1);
    }

    for(int i = 0; i < N; i++)
    {
        pthread_join(phil_tid[i], NULL);
    }
    // printf("All philosophers have eaten.\n");
}

void *phil_thread(void *num)
{
    while(1)
    {
        if(have_all_eaten()) break;
        int *i = (int *)num;
        sleep(3);
        pick_up_chop(*i);
        sleep(1);
        put_down_chop(*i);
    }
}

void test(int pnum)
{
    if(phil[pnum].state == HUNGRY 
    && phil[LEFT].state != EAT 
    && phil[RIGHT].state != EAT)
    {
        phil[pnum].state = EAT;
        sleep(1);
        printf("P%d receives F%d, F%d\n", pnum + 1, LEFT + 1, pnum + 1);
        // printf("P%d is eating\n", pnum + 1);
        sem_post(&chop_lock[pnum]);
    }
}

void pick_up_chop(int pnum)
{
    sem_wait(&mutex);
    phil[pnum].state = HUNGRY;
    // printf("P%d is hungry\n", pnum + 1);
    test(pnum);
    sem_post(&mutex);
    sem_wait(&chop_lock[pnum]);
    sleep(1);
}

void put_down_chop(int pnum)
{
    sem_wait(&mutex);
    phil[pnum].state = THINK;
    // printf("P%d puts down F%d, F%d\n", pnum + 1, LEFT + 1, pnum + 1);
    phil[pnum].eaten++;
    // printf("P%d is thinking\n", pnum + 1);
    test(LEFT);
    test(RIGHT);
    sem_post(&mutex);
}

int have_all_eaten()
{
    for(int i = 0; i < N; i++)
    {
        if(phil[i].eaten == 0) return 0;
    }
    return 1;
}