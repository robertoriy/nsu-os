#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef struct input_data 
{
    char* string;
    int length;
} input_data;

void* print_lines(void* argv);
int memory_allocation(const int size, pthread_t** pthreads, input_data** array);
void fill_input_data(input_data* in_data, int size, int argc, char** argv);

int main(int argc, char** argv)
{
    if (1 == argc || argc > 101)
    {
        fprintf(stderr, "Expected correct arguments\n");
        return EXIT_FAILURE;
    }
    int EXPECTED_NUMBER_OF_THREADS = argc - 1;
    int ACTUAL_NUMBER_OF_THREADS = 0;

    pthread_t* pthreads;
    input_data* in_data;
    if (EXIT_FAILURE == memory_allocation(EXPECTED_NUMBER_OF_THREADS, &pthreads, &in_data))
    {
        return EXIT_FAILURE;
    }
    fill_input_data(in_data, EXPECTED_NUMBER_OF_THREADS, argc, argv);

    for (int i = 0; i < EXPECTED_NUMBER_OF_THREADS; ++i)
    {
        if (EXIT_SUCCESS != pthread_create(&pthreads[i], NULL, print_lines, (void*) &in_data[i]))
        {
            perror("Failed to create a thread");
            fprintf(stderr, "Number of strings less than required\n");
            break;
        }
        ++ACTUAL_NUMBER_OF_THREADS;
    }

    for (int i = 0; i < ACTUAL_NUMBER_OF_THREADS; ++i)
    {
        int* return_value;
        if (EXIT_SUCCESS != pthread_join(pthreads[i], (void**) &return_value))
        {
            perror("Failed to join the thread");
            free(pthreads);
            free(in_data);
            return EXIT_FAILURE;
        }
        if ((void*)EXIT_FAILURE == return_value)
        {
            free(pthreads);
            free(in_data);
            return EXIT_FAILURE;
        }
    }
    free(pthreads);
    free(in_data);
    return EXIT_SUCCESS;
}

void* print_lines(void* argv)
{
	if (NULL == argv)
	{
		fprintf(stderr, "Wrong arguments for 'print_lines()'\n");
		return (void*) EXIT_FAILURE;
	}
	const input_data* const in_data = (const input_data* const) argv;

    usleep(1500 * in_data->length);
	printf("%s\n", in_data->string);

	return NULL;
}

int memory_allocation(const int size, pthread_t** pthreads, input_data** array)
{
    *pthreads = malloc(sizeof(pthreads) * size);
    if (NULL == *pthreads)
    {
        perror("malloc() failed");
        return EXIT_FAILURE;
    }
    *array = calloc(size, sizeof(input_data));
    if (NULL == *array)
    {
        perror("calloc() failed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void fill_input_data(input_data* in_data, int size, int argc, char** argv)
{
    for (int i = 0; i < size; ++i)
    {
        in_data[i].string = argv[i + 1];
        in_data[i].length = strlen(in_data[i].string);
    }
}
