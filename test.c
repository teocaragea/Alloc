#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#define NR_THREADS 3

pthread_mutex_t mtx;

sem_t sem;

int value = 0;



void barrier_point()

{

        pthread_mutex_lock(&mtx);
        value++;
        pthread_mutex_unlock(&mtx);

        if (value == NR_THREADS)
                sem_post(&sem);
        else
        {
          sem_wait(&sem);
          sem_post(&sem);
        }
}

void* tfun()
{
    /*int *tid = (int *)v;
    printf("%d a ajuns la bariera ", *tid);
    printf("\n");
    barrier_point();
    printf("%d plecat de la bariera  ", *tid);
    printf("\n");
    free_f(tid);
    return NULL;*/
    char *tid = malloc_f(7*sizeof(char));
    strcpy(tid, "alune ");
    printf("%s a ajuns la bariera ", tid);
    printf("\n");
    barrier_point();
    printf("%s plecat de la bariera  ", tid);
    printf("\n");
    free_f(tid);
    return NULL;
}

int main()
{
    int i = 0;
    int *current;
    pthread_mutex_init(&mtx, NULL);
    sem_init(&sem, 0, 0);
    pthread_t threads[NR_THREADS];
    while (i < NR_THREADS)
    {
        // current = malloc_f(sizeof(int));
	//*current = i;
	pthread_create(&threads[i], NULL, tfun, NULL);
        // pthread_create(&threads[i], NULL, tfun, current);
        i++;
    }
    i = 0;
    while (i < NR_THREADS)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    return 0;
}
