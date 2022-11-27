#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include "error_check.h"

#define THINKING_TIME 30000
#define EATING_TIME 50000
#define AMOUNT_OF_FOOD 1000

#define THINKING 0
#define HUNGRY 1
#define EATING 2

typedef struct philosopher_data
{
    int id;
    int eated;
} philosopher_data;

int NUMBER_OF_PHILOSOPHERS;
int *states;
sem_t* semaphores;
pthread_mutex_t states_mutex;
pthread_mutex_t food_mutex;

void* philosopher_action(void* argv);

bool can_eat(int* eated_value); 

void think(int id);
void take_forks(int id);
void eat(int id);
void put_forks(int id);

void check_forks(int id);
int left(int id);
int right(int id);

int memory_allocation(pthread_t** pthreads, philosopher_data** philosophers);
void fill_philosopher_info(philosopher_data* philosophers);
int init_resources();
void delete_all_resources(pthread_t* pthreads, philosopher_data* philosophers);
void destroy_sync_obj();
void free_resources(pthread_t* pthreads, philosopher_data* philosophers);


int main (int argc, char **argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }
    NUMBER_OF_PHILOSOPHERS = atoi(argv[1]);
    if (NUMBER_OF_PHILOSOPHERS < 1)
    {
        fprintf(stderr, "Expected value >= 1\n");
        return EXIT_FAILURE;
    }

    pthread_t* pthreads;
    philosopher_data* philosophers;

    CHCK_FUNC(memory_allocation(&pthreads, &philosophers), return EXIT_FAILURE);

    fill_philosopher_info(philosophers);

    CHCK_FUNC(init_resources(), free_resources(pthreads, philosophers); exit(EXIT_FAILURE));

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        CHCK_FUNC(pthread_create(&pthreads[i], NULL, philosopher_action, (void*) &philosophers[i]),\
        delete_all_resources(pthreads, philosophers); exit(EXIT_FAILURE));
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        void* error_check;

        CHCK_FUNC(pthread_join (pthreads[i], &error_check), \
        delete_all_resources(pthreads, philosophers); exit(EXIT_FAILURE));

        CHCK_THR_FAIL(error_check, delete_all_resources(pthreads, philosophers); exit(EXIT_FAILURE));
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        printf ("[Phil] - %d : eated - %d.\n", i, philosophers[i].eated);
    }

    delete_all_resources(pthreads, philosophers);
    return EXIT_SUCCESS;
}


void* philosopher_action(void* argv)
{
    CHCK_NULL(argv, return (void*)EXIT_FAILURE);
	philosopher_data* const philosopher = (philosopher_data* const) argv;
    
    printf ("[Phil] - %d : sitting down to dinner.\n", philosopher->id);


    while (can_eat(&(philosopher->eated))) 
    {
        think(philosopher->id);

        take_forks(philosopher->id);
        
        eat(philosopher->id);
        
        put_forks(philosopher->id);
    }
    printf ("[Phil] - %d : is done eating.\n", philosopher->id);
    return (NULL);
}

bool can_eat(int* eated_value) 
{
    static int food = AMOUNT_OF_FOOD;
    pthread_mutex_lock (&food_mutex);
    bool expression = food > 0;
    if (expression) 
    {
        --food;
        ++(*eated_value);
    }
    pthread_mutex_unlock (&food_mutex);
    return expression;
}

void think(int id)
{
    printf ("[Phil] - %d : is thinking\n", id);
    usleep(THINKING_TIME);
}

void take_forks(int id)
{
    pthread_mutex_lock(&states_mutex);

    states[id] = HUNGRY;

    check_forks(id);

    pthread_mutex_unlock(&states_mutex);

    printf ("[Phil] - %d : is trying to take forks\n", id);
    
    sem_wait(&semaphores[id]);
}

void eat(int id)
{
    printf ("[Phil] - %d : is eating\n", id);
    usleep(EATING_TIME); 
}

void put_forks(int id) 
{
    printf ("[Phil] - %d : is putting forks\n", id);

    pthread_mutex_lock(&states_mutex);

    states[id] = THINKING;

    check_forks(left(id));
    check_forks(right(id));

    pthread_mutex_unlock(&states_mutex);
}
void check_forks(int id)
{
    int left_one = left(id);
    int right_one = right(id);
    if (states[id] == HUNGRY && states[left_one] != EATING && states[right_one] != EATING) 
    {
        states[id] = EATING;
        sem_post(&semaphores[id]);
    }
}

int left(int id)
{
    return (id - 1 + NUMBER_OF_PHILOSOPHERS) % NUMBER_OF_PHILOSOPHERS;
}

int right(int id)
{
    return (id + 1) % NUMBER_OF_PHILOSOPHERS;
}


int memory_allocation(pthread_t** pthreads, philosopher_data** philosophers)
{
    *pthreads = calloc(NUMBER_OF_PHILOSOPHERS, sizeof(pthread_t));
    CHCK_NULL(*pthreads, return EXIT_FAILURE);

    *philosophers = calloc(NUMBER_OF_PHILOSOPHERS, sizeof(philosopher_data));
    CHCK_NULL(*philosophers, free(*pthreads); return EXIT_FAILURE);

    semaphores = calloc(NUMBER_OF_PHILOSOPHERS, sizeof(sem_t));
    CHCK_NULL(semaphores, free(*pthreads); free(*philosophers); return EXIT_FAILURE);

    states = calloc(NUMBER_OF_PHILOSOPHERS, sizeof(int));
    CHCK_NULL(states, free(*pthreads); free(*philosophers); free(semaphores); return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

int init_resources()
{
    CHCK_FUNC(pthread_mutex_init(&states_mutex, NULL), return EXIT_FAILURE);
    CHCK_FUNC(pthread_mutex_init(&food_mutex, NULL), pthread_mutex_destroy(&states_mutex); return EXIT_FAILURE);

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i)
    {
        CHCK_FUNC(sem_init(&semaphores[i], 0, 0), return EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

void fill_philosopher_info(philosopher_data* philosophers)
{
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i)
    {
        philosophers[i].id = i;
        philosophers[i].eated = 0;
    }
}

void delete_all_resources(pthread_t* pthreads, philosopher_data* philosophers)
{
    destroy_sync_obj();
    free_resources(pthreads, philosophers);
}

void destroy_sync_obj()
{
    pthread_mutex_destroy(&states_mutex);
    pthread_mutex_destroy(&food_mutex);
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i)
    {
        sem_destroy(&semaphores[i]);
    }
}

void free_resources(pthread_t* pthreads, philosopher_data* philosophers)
{
    free(pthreads);
    free(philosophers);
    free(semaphores);
    free(states);
}