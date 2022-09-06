#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_LINE 10

void* print_lines(void* argv);

int main()
{
	pthread_t thread;
	const char* const PARENT_TEXT = "parent";
	const char* const CHILD_TEXT = "child";
	
	if (0 != pthread_create(&thread, NULL, print_lines, (void*)CHILD_TEXT))
	{
		perror("Failed to create a thread");
		return EXIT_FAILURE;
	}
	
	print_lines((void*)PARENT_TEXT);
	
	pthread_exit(NULL);

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

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		printf("Written by %s - %d\n", line_to_print, i);
	}

	return NULL;
}

