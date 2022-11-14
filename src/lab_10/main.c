#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include "error_check.h"

#define NUMBER_OF_PHILOSOPHERS 5
#define DELAY 30000
#define AMOUNT_OF_FOOD 50

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[NUMBER_OF_PHILOSOPHERS];
sem_t semaphores[NUMBER_OF_PHILOSOPHERS];
pthread_mutex_t mutex;

void* philosopher(void* argv);

void think(void);
void take_forks(int id);
void eat(void);
void put_forks(int id);
void check_forks(int id);

bool can_eat();
int left(int id);
int right(int id);

void destroy_semaphores();


int main (int argc, char **argv)
{
    pthread_t philosophers[NUMBER_OF_PHILOSOPHERS];

    CHCK_FUNC(pthread_mutex_init (&mutex, NULL), return EXIT_FAILURE);

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        CHCK_FUNC(sem_init(&semaphores[i], 0, 0), return EXIT_FAILURE);
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        CHCK_FUNC(pthread_create(&philosophers[i], NULL, philosopher, (void*)i), \
        destroy_semaphores(); return EXIT_FAILURE);
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        CHCK_FUNC(pthread_join (philosophers[i], NULL), \
        destroy_semaphores(); return EXIT_FAILURE);
    }

    destroy_semaphores();
    return EXIT_SUCCESS;
}

// void* philosopher_base(void *num)
// {
//     int id;
//     int left_fork, right_fork, food_left;
//     id = (int)num;
//     printf ("Philosopher %d sitting down to dinner.\n", id);
//     right_fork = id;
//     left_fork = id + 1;
//     /* Wrap around the forks. */
//     if (left_fork == NUMBER_OF_PHILOSOPHERS)
//         left_fork = 0;

//     while (food_left = food_on_table ()) {
//         /* Thanks to philosophers #1 who would like to 
//             * take a nap before picking up the forks, the other
//             * philosophers may be able to eat their dishes and 
//             * not deadlock.
//             */
//         if (id == 1)
//             sleep (sleep_seconds);

//         printf ("Philosopher %d: get dish %d.\n", id, food_left);
//         get_fork (id, right_fork, "right");
//         get_fork (id, left_fork, "left ");

//         printf ("Philosopher %d: eating.\n", id);
//         usleep (DELAY * (FOOD - food_left + 1));
//         down_forks (left_fork, right_fork);
//     }
//     printf ("Philosopher %d is done eating.\n", id);
//     return (NULL);
// }

// int food_on_table_base()
// {
//     static int food = FOOD;
//     int myfood;
//     pthread_mutex_lock (&foodlock);
//     if (food > 0) {
//         food--;
//     }
//     myfood = food;
//     pthread_mutex_unlock (&foodlock);
//     return myfood;
// }

// void get_fork_base(int phil, int fork, char *hand)
// {
//     pthread_mutex_lock (&forks[fork]);
//     printf ("Philosopher %d: got %s fork %d\n", phil, hand, fork);
// }

// void down_forks_base(int f1, int f2)
// {
//     pthread_mutex_unlock (&forks[f1]);
//     pthread_mutex_unlock (&forks[f2]);
// }


void* philosopher(void* argv)
{
    CHCK_NULL(argv, return (void*)EXIT_FAILURE);
	const int MY_ID = (const int) argv;
    
    printf ("Philosopher %d sitting down to dinner.\n", MY_ID);

    int left_philosopher = left(MY_ID);
    int right_philosopher = right(MY_ID);

    while (can_eat()) 
    {
        printf ("Philosopher %d: is thinking\n", MY_ID);
        think();

        printf ("Philosopher %d: is trying to take forks\n", MY_ID);
        take_forks(MY_ID);
        
        printf ("Philosopher %d: is eating\n", MY_ID);
        eat();
        
        printf ("Philosopher %d: is putting forks\n", MY_ID);
        put_forks(MY_ID);
    }
    printf ("Philosopher %d is done eating.\n", MY_ID);
    return (NULL);
}

void think(void);
void take_forks(int id);
void eat(void);
void put_forks(int id);
void check_forks(int id);

bool can_eat() 
{
    return true;
}

int left(int id)
{
    return (id - 1 + NUMBER_OF_PHILOSOPHERS) % NUMBER_OF_PHILOSOPHERS;
}

int right(int id)
{
    return (id + 1) % NUMBER_OF_PHILOSOPHERS;
}

void destroy_semaphores()
{
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; ++i)
    {
        sem_destroy(&semaphores[i]);
    }
}