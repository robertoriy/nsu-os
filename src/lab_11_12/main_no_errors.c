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
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    mutexes_init(&attr);

    pthread_create(&child_thread, NULL, print_lines, (void*)CHILD_TEXT);

    pthread_mutex_lock(&mutexes[0]);
	while (!started)
	{
		usleep(300);
	}

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
        pthread_mutex_lock(&mutexes[(i + 1) % NUMBER_OF_MUTEXES]);
		printf("Written by %s - %d\n", PARENT_TEXT, i);
		pthread_mutex_unlock(&mutexes[i % NUMBER_OF_MUTEXES]);
	}
	pthread_mutex_unlock(&mutexes[(NUMBER_OF_LINE) % NUMBER_OF_MUTEXES]);

	pthread_join(child_thread, NULL);
    exit(EXIT_FAILURE);
}

void* print_lines(void* argv)
{
	const char* const line_to_print = (const char* const) argv;

	pthread_mutex_lock(&mutexes[NUMBER_OF_MUTEXES - 1]);
	started = true;

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{		
		pthread_mutex_lock(&mutexes[i % NUMBER_OF_MUTEXES]);
		printf("Written by %s - %d\n", line_to_print, i);
		pthread_mutex_unlock(&mutexes[(i + 2) % NUMBER_OF_MUTEXES]);
	}
	pthread_mutex_unlock(&mutexes[(NUMBER_OF_LINE - 1) % NUMBER_OF_MUTEXES]);

	return NULL;
}


int mutexes_init(pthread_mutexattr_t* attr)
{
	pthread_mutex_init(&mutexes[0], attr);
    pthread_mutex_init(&mutexes[1], attr);
    pthread_mutex_init(&mutexes[2], attr);
	return EXIT_SUCCESS;
}

void mutexes_destroy()
{
	for (int i = 0; i < NUMBER_OF_MUTEXES; ++i)
	{
		pthread_mutex_destroy(&mutexes[i]);
	}
}