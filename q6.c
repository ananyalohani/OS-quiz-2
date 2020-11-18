#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define TIME 1
#define LEFT pnum
#define RIGHT (pnum + 1) % N

struct philos 
{
    int num;
    int eaten;
    pthread_t phil_tid;
};

sem_t mutex;
sem_t chop[N];
struct philos phil[N];

void pick_left_chop(int pnum);
void pick_right_chop(int pnum);
void *phil_thread(void *num);
int have_all_eaten();

int main(int argc, char **argv)
{
    sem_init(&mutex, 0, 1);

    for(int i = 0; i < N; i++)
    {
        sem_init(&chop[i], 0, 0);
        phil[i].num = i; 
        phil[i].eaten = 0;
    }
    for(int i = 0; i < N; i++)
    {
        pthread_create(&phil[i].phil_tid, NULL, phil_thread, &phil[i].num);
        // printf("P%d is thinking\n", i + 1);
    }

    for(int i = 0; i < N; i++)
    {
        pthread_join(phil[i].phil_tid, NULL);
    }
    // printf("All philosophers have eaten.\n");
    sem_destroy(&mutex);
    for(int i = 0; i < N; i++)
    {
        sem_destroy(&chop[i]);
    }
    return 0;
}

void *phil_thread(void *num)
{
    while(1)
    {
        if(have_all_eaten()) break;
        int *i = (int *)num;
        sleep(TIME);    // thinking
        pick_left_chop(*i);
        pick_right_chop(*i);
    }
}

void pick_left_chop(int pnum)
{
    sem_wait(&mutex);
    sem_wait(&chop[LEFT]);
    printf("P%d receives F%d\n", phil[pnum].num + 1, LEFT);
}

void pick_right_chop(int pnum)
{
    sem_wait(&chop[RIGHT]);
    sem_post(&mutex);

    printf("P%d receives F%d, F%d\n", phil[pnum].num + 1, LEFT, RIGHT);
    phil[pnum].eaten++;
    sleep(TIME);    //eating

    sem_post(&chop[RIGHT]);
    sem_post(&chop[LEFT]);
}

int have_all_eaten()
{
    for(int i = 0; i < N; i++)
    {
        if(phil[i].eaten == 0) return 0;
    }
    return 1;
}