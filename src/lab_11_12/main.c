#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUMBER_OF_LINE 20
#define NUMBER_OF_MUTEXES 3

pthread_mutex_t m[NUMBER_OF_MUTEXES];

void* print_lines(void* argv);

int main()
{
	pthread_t child_thread;
	const char* const PARENT_TEXT = "parent";
	const char* const CHILD_TEXT = "child";

	pthread_mutexattr_t attr;
	if (EXIT_SUCCESS != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK))
	{
		fprintf(stderr, "pthread_mutexattr_settype() Failed\n");
		pthread_mutexattr_destroy(&attr);
		return EXIT_FAILURE;
	}

    for (int i = 0; i < NUMBER_OF_MUTEXES; ++i)
    {
        pthread_mutex_init(&m[i], &attr);
    }
	
	if (EXIT_SUCCESS != pthread_create(&child_thread, NULL, print_lines, (void*)CHILD_TEXT))
	{
		perror("Failed to create a thread");
		return EXIT_FAILURE;
	}



	pthread_mutex_lock(&m[NUMBER_OF_MUTEXES - 1]);

    usleep(500);

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		pthread_mutex_lock(&m[i % NUMBER_OF_MUTEXES]);
		printf("Written by %s - %d\n", PARENT_TEXT, i);
		pthread_mutex_unlock(&m[(i + 2) % NUMBER_OF_MUTEXES]);
	}

    pthread_mutex_unlock(&m[(NUMBER_OF_LINE - 2) % NUMBER_OF_MUTEXES]);




	if (EXIT_SUCCESS != pthread_join(child_thread, NULL))
	{
		perror("Failed to create a thread");
		return EXIT_FAILURE;
	}

	pthread_mutex_destroy(&m[0]);
    pthread_mutex_destroy(&m[1]);
	pthread_mutex_destroy(&m[2]);
	pthread_mutexattr_destroy(&attr);

	return EXIT_SUCCESS;
}

void* print_lines(void* argv)
{
	if (NULL == argv)
	{
		fprintf(stderr, "Wrong arguments for 'print_lines()'\n");
		pthread_exit((void*)EXIT_FAILURE);
	}

	const char* const line_to_print = (const char* const) argv;



    pthread_mutex_lock(&m[0]);

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		pthread_mutex_lock(&m[(i + 1) % NUMBER_OF_MUTEXES]);
		printf("Written by %s - %d\n", line_to_print, i);
		pthread_mutex_unlock(&m[i % NUMBER_OF_MUTEXES]);
	}

    pthread_mutex_unlock(&m[(NUMBER_OF_LINE - 1) % NUMBER_OF_MUTEXES]);


	return NULL;
}
