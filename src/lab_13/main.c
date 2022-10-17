#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "error_check.h"

#define NUMBER_OF_THREADS 2
#define PARENT_ID 0
#define CHILD_ID 1

int NUMBER_OF_LINES = 0;
pthread_mutex_t mutex;
pthread_cond_t condition;
int TURN = PARENT_ID;

typedef struct thread_data
{
    int id;
    char* text;
} thread_data;

void* print_lines(void* argv);
void destroy_attr(pthread_mutex_t* m, pthread_cond_t* c);

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

    CHECK_FUNCTION(pthread_mutex_init(&mutex, NULL), return EXIT_FAILURE);
    CHECK_FUNCTION(pthread_cond_init(&condition, NULL), pthread_mutex_destroy(&mutex); return EXIT_FAILURE);

    pthread_t child_thread;
	CHECK_FUNCTION(pthread_create(&child_thread, NULL, print_lines, (void*)&child_data), \
    destroy_attr(&mutex, &condition); return EXIT_FAILURE);

    CHECK_THREAD_ARGUMENTS(print_lines((void*)&parent_data), \
    destroy_attr(&mutex, &condition); return EXIT_FAILURE);

    void* error_check;
    CHECK_FUNCTION(pthread_join(child_thread, &error_check), \
    destroy_attr(&mutex, &condition); return EXIT_FAILURE);
    CHECK_THREAD_ARGUMENTS(error_check, destroy_attr(&mutex, &condition); return EXIT_FAILURE);

    destroy_attr(&mutex, &condition);
	return EXIT_SUCCESS;
}

void* print_lines(void* argv)
{
	CHECK_NULL(argv, return (void*)EXIT_FAILURE);
	const thread_data* const data = (const thread_data* const) argv;

	for (int i = 0; i < NUMBER_OF_LINES; ++i)
	{
        CHECK_SYNCHR_OBJ(pthread_mutex_lock(&mutex));
        while (TURN != data->id)
        {
            CHECK_SYNCHR_OBJ(pthread_cond_wait(&condition, &mutex));
        }

		printf("Written by %s - %d\n", data->text, i + 1);
        if (CHILD_ID == data->id)
        {
            printf("\n");
        }

        TURN = (data->id == PARENT_ID ? CHILD_ID : PARENT_ID);
        CHECK_SYNCHR_OBJ(pthread_mutex_unlock(&mutex));
        CHECK_SYNCHR_OBJ(pthread_cond_signal(&condition));
	}
	return NULL;
}

void destroy_attr(pthread_mutex_t* m, pthread_cond_t* c)
{
    pthread_mutex_destroy(m);
    pthread_cond_destroy(c);
}