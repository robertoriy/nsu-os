#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_LINE 10
#define PARENT_TEXT "parent" 
#define CHILD_TEXT "child"

void* print_lines(void* args);

int main()
{
	pthread_t thread;
	
	if (0 != pthread_create(&thread, NULL, print_lines, CHILD_TEXT))
	{
		perror("Failed to create a thread");
		return EXIT_FAILURE;
	}
	
	print_lines(PARENT_TEXT);
	
	pthread_exit(NULL);
	return EXIT_SUCCESS;
}

void* print_lines(void* args)
{
	if (NULL == args)
	{
		fprintf(stderr, "Wrong arguments for 'print_lines()'\n");
		exit(EXIT_FAILURE);
	}

	char* string = (char*) args;

	for (int i = 0; i < NUMBER_OF_LINE; ++i)
	{
		printf("Written by %s - %d\n", string, i);
	}
	return NULL;
}

