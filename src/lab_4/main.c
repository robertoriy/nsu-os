#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUMBER_OF_LINE 100000
#define TIME_TO_SLEEP 2

void* print_lines(void* argv);

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
	time_t now;
	struct tm* time_attr;

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		time(&now);
		time_attr = localtime(&now);
		printf("Written by %s - %d\n", line_to_print, time_attr->tm_sec);
	}

	return NULL;
}



