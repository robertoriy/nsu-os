#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_THREADS 4
#define NUMBER_OF_LINE 17

void* print_lines(void* argv);

int main()
{
	pthread_t pthread_array[NUMBER_OF_THREADS];

    const char* const TEXT[NUMBER_OF_THREADS] = { 
                        "first thread",
                        "second thread", 
                        "third thread",
                        "fourth thread"
                    };

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        if (0 != pthread_create(&pthread_array[i], NULL, print_lines, (void*)TEXT[i]))
        {
            perror("Failed to create a thread");
            exit(EXIT_FAILURE);
        }
    } 

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        if (0 != pthread_join(pthread_array[i], NULL))
        {
            perror("Failed to join the thread");
            exit(EXIT_FAILURE);
        }
    }

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

