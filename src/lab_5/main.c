#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUMBER_OF_LINE 100000
#define TIME_TO_SLEEP 2
#define CLEANUP_POP_ARG 1

bool time_is_over = false;

void* print_lines(void* argv);
void cleanup_handler(void* argv);

int main()
{
	pthread_t child_thread;
	const char* const PARENT_TEXT = "parent. Time is over";
	const char* const CHILD_TEXT = "child. Hello, world!";
	
	if (0 != pthread_create(&child_thread, NULL, print_lines, (void*)CHILD_TEXT))
	{
		perror("Failed to create a thread");
		return EXIT_FAILURE;
	}

	sleep(TIME_TO_SLEEP);
    time_is_over = true;

	if (0 != pthread_cancel(child_thread))
    {
        perror("Failed to cancel the thread");
		return EXIT_FAILURE;
    }

	printf("\nWritten by %s\n", PARENT_TEXT);
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

    pthread_cleanup_push(cleanup_handler, NULL);
    
    time_t current_time = time(NULL);
	struct tm* time_attr;

    while (!time_is_over) 
    {
        if (current_time < time(NULL)) 
        {
            current_time = time(NULL);
            time_attr = localtime(&current_time);
            printf("Written by %s. Time in sec - %d\n", line_to_print, time_attr->tm_sec);
        }
    }
    pthread_cleanup_pop(CLEANUP_POP_ARG);
	return NULL;
}

void cleanup_handler(void* argv)
{
    printf("\nWritten by child. Called cancel\n");
}
       