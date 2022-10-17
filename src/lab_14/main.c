#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "error_check.h"

#define NUMBER_OF_THREADS 2
#define PARENT_ID 0
#define CHILD_ID 1

int NUMBER_OF_LINES = 0;
sem_t semaphores[NUMBER_OF_THREADS];

typedef struct thread_data
{
    int id;
    char* text;
} thread_data;

void* print_lines(void* argv);
void destroy_semaphores();

int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }
    NUMBER_OF_LINES = atoi(argv[1]);
    if (NUMBER_OF_LINES < 1 || NUMBER_OF_LINES > 50000)
    {
        fprintf(stderr, "Expected correct argument: [1:50000]\n");
        return EXIT_FAILURE;
    }

    char* PARENT_TEXT = "[parent]";
	char* CHILD_TEXT = "[child]";
    thread_data parent_data = {PARENT_ID, PARENT_TEXT};
    thread_data child_data = {CHILD_ID, CHILD_TEXT};

    CHECK_FUNCTION(sem_init(&semaphores[PARENT_ID], 0, 0), return EXIT_FAILURE);
    CHECK_FUNCTION(sem_init(&semaphores[CHILD_ID], 0, 1), sem_destroy(&semaphores[PARENT_ID]); return EXIT_FAILURE);

    pthread_t child_thread;
	CHECK_FUNCTION(pthread_create(&child_thread, NULL, print_lines, (void*)&child_data), \
    destroy_semaphores(); return EXIT_FAILURE);

    CHECK_THREAD_ARGUMENTS(print_lines((void*)&parent_data), \
    destroy_semaphores(); return EXIT_FAILURE);

    void* error_check;
    CHECK_FUNCTION(pthread_join(child_thread, &error_check), \
    destroy_semaphores(); return EXIT_FAILURE);
    CHECK_THREAD_ARGUMENTS(error_check, destroy_semaphores(); return EXIT_FAILURE);

    destroy_semaphores();
	return EXIT_SUCCESS;
}

void* print_lines(void* argv)
{
	CHECK_NULL(argv, return (void*)EXIT_FAILURE);
	const thread_data* const data = (const thread_data* const) argv;

	for (int i = 0; i < NUMBER_OF_LINES; ++i)
	{
        CHECK_SYNCHR_OBJ(sem_wait(&semaphores[data->id == PARENT_ID ? CHILD_ID : PARENT_ID]));

		printf("Written by %s - %d\n", data->text, i + 1);
        if (CHILD_ID == data->id)
        {
            printf("\n");
        }
        
        CHECK_SYNCHR_OBJ(sem_post(&semaphores[data->id == PARENT_ID ? PARENT_ID : CHILD_ID]));
	}
	return NULL;
}

void destroy_semaphores()
{
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        sem_destroy(&semaphores[i]);
    }
}
