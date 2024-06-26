#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUMBER_OF_LINE 20
#define NUMBER_OF_MUTEXES 3

pthread_mutex_t mutexes[NUMBER_OF_MUTEXES];
bool started = false;

void* print_lines(void* argv);
int mutexes_init(pthread_mutexattr_t* attr);
void mutexes_destroy();

int main()
{
	pthread_t child_thread;
	const char* const PARENT_TEXT = "parent";
	const char* const CHILD_TEXT = "child";

	pthread_mutexattr_t attr;
	if (EXIT_SUCCESS != pthread_mutexattr_init(&attr))
	{
		perror("pthread_mutexattr_init() failed");
		return EXIT_FAILURE;
	}
	if (EXIT_SUCCESS != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK))
	{
		perror("pthread_mutexattr_settype() failed");
		pthread_mutexattr_destroy(&attr);
		return EXIT_FAILURE;
	}

    if (EXIT_FAILURE == mutexes_init(&attr))
	{
		fprintf(stderr, "mutex_init() failed\n");
		pthread_mutexattr_destroy(&attr);
		return EXIT_FAILURE;
	}

    if (EXIT_SUCCESS != pthread_create(&child_thread, NULL, print_lines, (void*)CHILD_TEXT))
	{
		perror("Failed to create a thread");
		mutexes_destroy();
		pthread_mutexattr_destroy(&attr);
		return EXIT_FAILURE;
	}

    if (EXIT_SUCCESS != pthread_mutex_lock(&mutexes[0]))
	{
		exit(EXIT_FAILURE);
	}
	while (!started)
	{
		usleep(300);
	}

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		if (EXIT_SUCCESS != pthread_mutex_lock(&mutexes[(i + 1) % NUMBER_OF_MUTEXES]))
		{
			exit(EXIT_FAILURE);
		}
		printf("Written by %s - %d\n", PARENT_TEXT, i);
		if (EXIT_SUCCESS != pthread_mutex_unlock(&mutexes[i % NUMBER_OF_MUTEXES]))
		{
			exit(EXIT_FAILURE);
		}
	}
	if (EXIT_SUCCESS != pthread_mutex_unlock(&mutexes[(NUMBER_OF_LINE) % NUMBER_OF_MUTEXES]))
	{
		exit(EXIT_FAILURE);
	}

	if (EXIT_SUCCESS != pthread_join(child_thread, NULL))
	{
		perror("Failed to create a thread");
		mutexes_destroy();
		pthread_mutexattr_destroy(&attr);
		return EXIT_FAILURE;
	}

	mutexes_destroy();
	pthread_mutexattr_destroy(&attr);

	return EXIT_SUCCESS;
}

void* print_lines(void* argv)
{
	if (NULL == argv)
	{
		fprintf(stderr, "Wrong arguments for 'print_lines()'\n");
		exit(EXIT_FAILURE);
	}
	const char* const line_to_print = (const char* const) argv;

	if (EXIT_SUCCESS != pthread_mutex_lock(&mutexes[NUMBER_OF_MUTEXES - 1]))
	{
		exit(EXIT_FAILURE);
	}
	started = true;

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{		
		if (EXIT_SUCCESS != pthread_mutex_lock(&mutexes[i % NUMBER_OF_MUTEXES]))
		{
			exit(EXIT_FAILURE);
		}
		printf("Written by %s - %d\n", line_to_print, i);
		if (EXIT_SUCCESS != pthread_mutex_unlock(&mutexes[(i + 2) % NUMBER_OF_MUTEXES]))
		{
			exit(EXIT_FAILURE);
		}
	}
	if (EXIT_SUCCESS != pthread_mutex_unlock(&mutexes[(NUMBER_OF_LINE - 1) % NUMBER_OF_MUTEXES]))
	{
		exit(EXIT_FAILURE);
	}

	return NULL;
}


int mutexes_init(pthread_mutexattr_t* attr)
{
	if (EXIT_SUCCESS != pthread_mutex_init(&mutexes[0], attr))
	{
		return EXIT_FAILURE;
	}
	if (EXIT_SUCCESS != pthread_mutex_init(&mutexes[1], attr))
	{
		pthread_mutex_destroy(&mutexes[1]);
		return EXIT_FAILURE;
	}
	if (EXIT_SUCCESS != pthread_mutex_init(&mutexes[2], attr))
	{
		pthread_mutex_destroy(&mutexes[1]);
		pthread_mutex_destroy(&mutexes[2]);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void mutexes_destroy()
{
	for (int i = 0; i < NUMBER_OF_MUTEXES; ++i)
	{
		pthread_mutex_destroy(&mutexes[i]);
	}
}