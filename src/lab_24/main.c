#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <semaphore.h>
#include "error_check.h"

#define NUMBER_OF_THREADS 5

#define PERIOD_OF_PRINTING 1

#define TIME_TO_CREATE_A 1
#define TIME_TO_CREATE_B 2
#define TIME_TO_CREATE_C 3
#define DEFAULT_TIME 0

#define DETAIL_A 0
#define DETAIL_B 1
#define DETAIL_C 2
#define MODULE 3
#define WIDGET 4

typedef struct producer_data
{
    int detail_type;
    int time_to_produce;
} producer_data;

sem_t detail_A;
sem_t detail_B;
sem_t detail_C;
sem_t module;
sem_t widget;

void* producer_routine(void* argv);
void produce(int type);

void fill_producers_data(producer_data* producer_info);
void print_produced_elemnts(int* produced_widgets);

void init_semaphores(void);
void destroy_semaphores(void);


int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }
    int REQUIRED_WIDGETS = atoi(argv[1]);
    if (REQUIRED_WIDGETS < 1 || REQUIRED_WIDGETS > 30)
    {
        fprintf(stderr, "Expected correct argument: [1:30]\n");
        return EXIT_FAILURE;
    }

    pthread_t producers[NUMBER_OF_THREADS];

    producer_data producer_info[NUMBER_OF_THREADS];
    fill_producers_data(producer_info);

    init_semaphores();

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        CHCK_FUNC(pthread_create(&producers[i], NULL, producer_routine, (void*) &producer_info[i]), \
        destroy_semaphores(); return EXIT_FAILURE);
    }

    int produced_widgets = 0;
    while (produced_widgets < REQUIRED_WIDGETS) 
    {
        sleep(PERIOD_OF_PRINTING);
        print_produced_elemnts(&produced_widgets);
    }

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        CHCK_FUNC(pthread_cancel(producers[i]), destroy_semaphores(); return EXIT_FAILURE);
    }

    destroy_semaphores();
	return EXIT_SUCCESS;
}


void* producer_routine(void* argv)
{
    CHCK_NULL(argv, return (void*)EXIT_FAILURE);
    const producer_data* const producer = (const producer_data* const) argv;

    while (true)
    {
        sleep(producer->time_to_produce);
        produce(producer->detail_type);
    }

	return NULL;
}

void produce(int type)
{   
    if (type == DETAIL_A)
    {
        sem_post(&detail_A);
        return;
    }
    if (type == DETAIL_B)
    {
        sem_post(&detail_B);
        return;
    }
    if (type == DETAIL_C)
    {
        sem_post(&detail_C);
        return;
    }
    if (type == MODULE)
    {
        sem_wait(&detail_B);
        sem_wait(&detail_A);
        sem_post(&module);
        return;
    }
    if (type == WIDGET)
    {
        sem_wait(&detail_C);
        sem_wait(&module);
        sem_post(&widget);
        return;
    }
}

void fill_producers_data(producer_data* producer_info)
{
    producer_info[DETAIL_A].detail_type = DETAIL_A;
    producer_info[DETAIL_A].time_to_produce = TIME_TO_CREATE_A;

    producer_info[DETAIL_B].detail_type = DETAIL_B;
    producer_info[DETAIL_B].time_to_produce = TIME_TO_CREATE_B;

    producer_info[DETAIL_C].detail_type = DETAIL_C;
    producer_info[DETAIL_C].time_to_produce = TIME_TO_CREATE_C;

    producer_info[MODULE].detail_type = MODULE;
    producer_info[MODULE].time_to_produce = DEFAULT_TIME;

    producer_info[WIDGET].detail_type = WIDGET;
    producer_info[WIDGET].time_to_produce = DEFAULT_TIME;
}

void print_produced_elemnts(int* widget_counter)
{
    int detail_A_counter;
    int detail_B_counter;
    int detail_C_counter;
    int module_counter;

    CHCK_FUNC(sem_getvalue(&detail_A, &detail_A_counter), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_getvalue(&detail_B, &detail_B_counter), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_getvalue(&detail_C, &detail_C_counter), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_getvalue(&module, &module_counter), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_getvalue(&widget, widget_counter), exit(EXIT_FAILURE));

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n >NUMBER OF PRODUCED DATAILS:\n");
    printf("\tTYPE : A\t- %d\n", detail_A_counter);
    printf("\tTYPE : B\t- %d\n", detail_B_counter);
    printf("\tTYPE : C\t- %d\n", detail_C_counter);
    printf("\tTYPE : MODULE   - %d\n", module_counter);
    printf("\tTYPE : WIDGET   - %d\n", *widget_counter);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void init_semaphores(void)
{
    CHCK_FUNC(sem_init(&detail_A, 0, 0), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_init(&detail_B, 0, 0), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_init(&detail_C, 0, 0), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_init(&module, 0, 0), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_init(&widget, 0, 0), exit(EXIT_FAILURE));
}

void destroy_semaphores(void)
{
    CHCK_FUNC(sem_destroy(&detail_A), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_destroy(&detail_B), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_destroy(&detail_C), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_destroy(&module), exit(EXIT_FAILURE));
    CHCK_FUNC(sem_destroy(&widget), exit(EXIT_FAILURE));
}

