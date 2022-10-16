#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include "error_check.h"

#define CHECK_STEP 1000000

int NUMBER_OF_THREADS = 0;
long long MAX_ITERATION = CHECK_STEP;
pthread_mutex_t mutex;
bool exit_flag = false;

typedef struct thread_data
{
    long long start;
    double local_result;
} thread_data;

void* pi_counter(void* argv);
void count_to_the_end(long long start_multiplier, long long number_of_iterations, double* result);
int set_sigint_handler();
void signal_handler(int signal);
int memory_allocation(pthread_t** pthreads, thread_data** thread_info);
void destroy_all_elements(pthread_mutex_t mutex, pthread_t* pthreads, thread_data* thread_info);

int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }
    NUMBER_OF_THREADS = atoi(argv[1]);
    if (NUMBER_OF_THREADS < 1)
    {
        fprintf(stderr, "Expected value >= 1\n");
        return EXIT_FAILURE;
    }

    pthread_t* pthreads;
    thread_data* thread_info;
    CHECK_FUNCTION(memory_allocation(&pthreads, &thread_info), return EXIT_FAILURE);

    CHECK_FUNCTION(pthread_mutex_init(&mutex, NULL), free(pthreads); free(thread_info); return EXIT_FAILURE);

    CHECK_FUNCTION(set_sigint_handler(), destroy_all_elements(mutex, pthreads, thread_info); return EXIT_FAILURE);

    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        thread_info[i].start = i;

        CHECK_FUNCTION(pthread_create(&pthreads[i], NULL, pi_counter, (void*) &thread_info[i]), \
        destroy_all_elements(mutex, pthreads, thread_info); return EXIT_FAILURE);
    }

    double pi_part = 0;
    double* thread_returned;
    for (int i = 0; i < NUMBER_OF_THREADS; ++i)
    {
        CHECK_FUNCTION(pthread_join(pthreads[i], (void**)&thread_returned), \
        destroy_all_elements(mutex, pthreads, thread_info); return EXIT_FAILURE);

        CHECK_THREAD_ARGUMENTS(thread_returned, destroy_all_elements(mutex, pthreads, thread_info); return EXIT_FAILURE);

        pi_part += thread_info[i].local_result;
    }

    printf("\n\tPi ~ %.15g\n", pi_part * 4);

    destroy_all_elements(mutex, pthreads, thread_info);

    return EXIT_SUCCESS;
}

void* pi_counter(void* argv)
{
    CHECK_NULL(argv, return (void*)EXIT_FAILURE);
	thread_data* const thread_info = (thread_data* const) argv;

    long long current_iteration = 0;
    for (long long i = thread_info->start; true; i += NUMBER_OF_THREADS)
    {
        thread_info->local_result += 1.0 / (i * 4.0 + 1.0) - 1.0 / (i * 4.0 + 3.0);
        ++current_iteration;

        if (current_iteration % CHECK_STEP == 0)
        {        
            CHECK_MUTEX(pthread_mutex_lock(&mutex));
            if (current_iteration > MAX_ITERATION)
            {
                MAX_ITERATION = current_iteration;
            }
            CHECK_MUTEX(pthread_mutex_unlock(&mutex));

            if (true == exit_flag)
            {
                if (current_iteration < MAX_ITERATION)
                {
                    printf("\n[tid - %lu] {iter - %lld} is going to count more\n", pthread_self(), current_iteration);
                    count_to_the_end(i + 1, MAX_ITERATION - current_iteration, &thread_info->local_result);
                    current_iteration = MAX_ITERATION;
                }
                printf("\n[tid - %lu] {iter - %lld} exited\n", pthread_self(), current_iteration);
                pthread_exit(NULL);
            }  
        }
    }
    return NULL;
}

void count_to_the_end(long long start_multiplier, long long number_of_iterations, double* result)
{
    long long multiplier = start_multiplier;
    for (long long i = 0; i < number_of_iterations; ++i)
    {
        *result += 1.0 / (multiplier * 4.0 + 1.0) - 1.0 / (multiplier * 4.0 + 3.0);
        multiplier += NUMBER_OF_THREADS;
    }
}

int set_sigint_handler()
{
    struct sigaction sigint_handler;
    memset(&sigint_handler, 0 , sizeof(struct sigaction));

    sigint_handler.sa_handler = signal_handler;

    CHECK_FUNCTION(sigaction(SIGINT, &sigint_handler, NULL), return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

void signal_handler(int signal)
{
    exit_flag = true;
}

int memory_allocation(pthread_t** pthreads, thread_data** thread_info)
{
    *pthreads = malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);
    CHECK_NULL(*pthreads, return EXIT_FAILURE);

    *thread_info = calloc(NUMBER_OF_THREADS, sizeof(thread_data));
    CHECK_NULL(*thread_info, free(*pthreads); return EXIT_FAILURE);

    return EXIT_SUCCESS;
}

void destroy_all_elements(pthread_mutex_t mutex, pthread_t* pthreads, thread_data* thread_info)
{
    pthread_mutex_destroy(&mutex);
    free(pthreads);
    free(thread_info);
}