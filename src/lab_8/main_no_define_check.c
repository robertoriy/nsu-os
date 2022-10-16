#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMBER_OF_STEPS 2000000000

typedef struct thread_data
{
    long long start;
    long long number_of_iterations;
} thread_data;


void* pi_counter(void* argv);
void fill_displacements_data(const int NUMBER_OF_THREADS, thread_data* displacements);
int memory_allocation(const int NUMBER_OF_THREADS, pthread_t** pthreads, thread_data** displacements);

int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }

    int EXPECTED_NUMBER_OF_THREADS = atoi(argv[1]);
    int ACTUAL_NUMBER_OF_THREADS = 0;

    if (EXPECTED_NUMBER_OF_THREADS < 1)
    {
        fprintf(stderr, "Expected value >= 1\n");
        return EXIT_FAILURE;
    }

    pthread_t* pthreads;
    thread_data* displacements;

    if (EXIT_FAILURE == memory_allocation(EXPECTED_NUMBER_OF_THREADS, &pthreads, &displacements))
    {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    fill_displacements_data(EXPECTED_NUMBER_OF_THREADS, displacements);

    for (int i = 0; i < EXPECTED_NUMBER_OF_THREADS; ++i)
    {
        if (0 != pthread_create(&pthreads[i], NULL, pi_counter, (void*) &displacements[i]))
        {
            perror("Failed to create a thread");
            fprintf(stderr, "Number of steps less than required\n");
            break;
        }
        ++ACTUAL_NUMBER_OF_THREADS;
    }

    double pi_divided_by_4 = 0;
    double* local_result;
    for (int i = 0; i < ACTUAL_NUMBER_OF_THREADS; ++i)
    {
        if (0 != pthread_join(pthreads[i], (void**)&local_result))
        {
            perror("Failed to join the thread");
            free(pthreads);
            free(displacements);
            return EXIT_FAILURE;
        }
        if ((void*)EXIT_FAILURE == local_result)
        {
            free(pthreads);
            free(displacements);
            return EXIT_FAILURE;
        }
        pi_divided_by_4 += (*local_result);
        free(local_result);
    }

    printf("Pi ~ %.15g\n", pi_divided_by_4 * 4);

    free(pthreads);
    free(displacements);

    return EXIT_SUCCESS;
}


void* pi_counter(void* argv)
{
    if (NULL == argv)
	{
		fprintf(stderr, "Wrong arguments for 'print_lines()'\n");
		return (void*)EXIT_FAILURE;
	}
	const thread_data* const displacement = (const thread_data* const) argv;

    double* local_result = calloc(1, sizeof(double));
    if (NULL == local_result)
    {
        perror("calloc() failed");
        return (void*)EXIT_FAILURE;
    }

    for (long long i = displacement->start; i < displacement->number_of_iterations; ++i)
    {
        *local_result += 1.0 / (i * 4.0 + 1.0);
        *local_result -= 1.0 / (i * 4.0 + 3.0);
    }
    return (void*)local_result;
}


void fill_displacements_data(const int NUMBER_OF_THREADS, thread_data* displacements)
{
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        displacements[i].number_of_iterations = NUMBER_OF_STEPS / NUMBER_OF_THREADS;
    }

    for (int i = 0; i < NUMBER_OF_STEPS % NUMBER_OF_THREADS; ++i)
    {
        ++displacements[i].number_of_iterations;
    }

    int displacement_index = 0;
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        displacements[i].start = displacement_index;
        displacement_index += displacements[i].number_of_iterations; 
    }
}

int memory_allocation(const int NUMBER_OF_THREADS, pthread_t** pthreads, thread_data** displacements)
{
    *pthreads = malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);
    if (NULL == *pthreads)
    {
        perror("malloc() failed");
        return EXIT_FAILURE;
    }

    *displacements = malloc(sizeof(thread_data) * NUMBER_OF_THREADS);
    if (NULL == *displacements)
    {
        perror("malloc() failed");
        free(*pthreads);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
