#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMBER_OF_STEPS 2000000000
#define CHECK_FUNCTION(function, action_on_error) do \
        { \
            int return_value = (function); \
            if (EXIT_SUCCESS != return_value) { \
                fprintf(stderr, "Runtime error: %s returned %d at %s:%d\n", #function, return_value, __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)

#define CHECK_THREAD_ARGUMENTS(return_value, action_on_error) do \
        { \
            if ((void*)EXIT_FAILURE == return_value) { \
                fprintf(stderr, "Runtime error: Thread have got wrong arguments. Error at %s:%d\n", __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)

#define CHECK_NULL(value, action_on_error) do\
        { \
            if (NULL == value) { \
                fprintf(stderr, "Runtime error: Unexpected NULL occured at %s:%d\n", __FILE__, __LINE__); \
                action_on_error; \
            } \
        } while (0)

typedef struct thread_data
{
    long long start;
    long long number_of_iterations;
    double local_result;
} thread_data;


void* pi_counter(void* argv);
void fill_displacements_data(const int NUMBER_OF_THREADS, thread_data* const displacements);
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

    CHECK_FUNCTION(memory_allocation(EXPECTED_NUMBER_OF_THREADS, &pthreads, &displacements), return EXIT_FAILURE);

    fill_displacements_data(EXPECTED_NUMBER_OF_THREADS, displacements);

    for (int i = 0; i < EXPECTED_NUMBER_OF_THREADS; ++i)
    {
        CHECK_FUNCTION(pthread_create(&pthreads[i], NULL, pi_counter, (void*) &displacements[i]), break);
        ++ACTUAL_NUMBER_OF_THREADS;
    }

    double pi_part = 0;
    double* local_result;
    for (int i = 0; i < ACTUAL_NUMBER_OF_THREADS; ++i)
    {
        CHECK_FUNCTION(pthread_join(pthreads[i], (void**)&local_result), free(pthreads); free(displacements); return EXIT_FAILURE);
        CHECK_THREAD_ARGUMENTS(local_result, free(pthreads); free(displacements); return EXIT_FAILURE);
        pi_part += displacements[i].local_result;
    }

    printf("Pi ~ %.15g\n", pi_part * 4);

    free(pthreads);
    free(displacements);

    return EXIT_SUCCESS;
}


void* pi_counter(void* argv)
{
    CHECK_NULL(argv, return (void*)EXIT_FAILURE);
	thread_data* const displacement = (thread_data* const) argv;

    for (long long i = displacement->start; i < displacement->number_of_iterations; ++i)
    {
        displacement->local_result += 1.0 / (i * 4.0 + 1.0);
        displacement->local_result -= 1.0 / (i * 4.0 + 3.0);
    }
    return NULL;
}


void fill_displacements_data(const int NUMBER_OF_THREADS, thread_data* const displacements)
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
    CHECK_NULL(*pthreads, return EXIT_FAILURE);

    *displacements = calloc(NUMBER_OF_THREADS, sizeof(thread_data));
    CHECK_NULL(*displacements, free(*pthreads); return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
