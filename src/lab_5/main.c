#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define TIME_TO_SLEEP 2
#define CLEANUP_POP_ARG 0

void* print_lines(void* argv);
void cleanup_handler(void* argv);

int main()
{
	pthread_t child_thread;
	if (0 != pthread_create(&child_thread, NULL, print_lines, NULL))
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

	printf("\nWritten by parrent. Time is over!\n");
    pthread_exit(EXIT_SUCCESS);
}

void* print_lines(void* argv)
{
    pthread_cleanup_push(cleanup_handler, NULL);
    
    time_t current_time = time(NULL);
	struct tm* time_attr;

    while(true)
    {
        if (current_time < time(NULL)) 
        {
            current_time = time(NULL);
            time_attr = localtime(&current_time);
            printf("Written by child. Time in sec - %d\n", time_attr->tm_sec);
        }
    }

    pthread_cleanup_pop(CLEANUP_POP_ARG);
    return NULL;
}

void cleanup_handler(void* argv)
{
    printf("\nWritten by child. Called cancel\n");
}
       