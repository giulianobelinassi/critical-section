/*Used for testing only. No need to touch this file*/
#include "globals.h"
#include "general_lock.h"

#include "bakery.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define QUANTUM 100

#define NUM_THREADS 100
#define NUM_EXECUTIONS 30
#define TOTAL_TIME 50000L

static int threads_in_cs = 0;
static bool privacy_violated = false;

void* consume_cpu(void* args)
{
    long my_total_time = TOTAL_TIME;
    int my_id = (int) args;
    volatile int i;

    while (my_total_time > 0)
    {
        lock_lock(my_id);
        {
            /* If this is a critical section, then this counter must not
             * be greater than 1. The contraposition of this statement
             * provides a way to test if the CS algorithm works.
             */
            threads_in_cs++;
            if (threads_in_cs != 1)
            {
                privacy_violated = true;
            }
            for (i = 0; i < QUANTUM; ++i)
            {
                threads_in_cs++;
                threads_in_cs--;
            }
            my_total_time -= i;
            
            if (threads_in_cs != 1)
            {
                privacy_violated = true;
            }
            threads_in_cs--;
        }
        lock_unlock(my_id);
    }

    return NULL;
}

int main(void)
{
    static pthread_t threads[MAX_PROCESSES];
    const lock_alg_t* algorithms = lock_get_all_algorithms();
    int num_algorithms = lock_get_number_of_algorithms();

    int i, j, k;

    for (j = 0; j < num_algorithms; ++j)
    {
        lock_alg_t algorithm = algorithms[j];
        const char* name = lock_get_algorithm_name(algorithm);

        privacy_violated = false;

        for (k = 0; k < NUM_EXECUTIONS; ++k)
        {
            threads_in_cs = 0;

            if (lock_init(NUM_THREADS, algorithm))
            {
                printf("Too much threads that I panicked!\n");
                return 2;
            }

            for (i = 0; i < NUM_THREADS; ++i)
            {
                if (pthread_create(&threads[i], NULL, &consume_cpu, (void*) i))
                {
                    printf("Unable to create thread %d, exiting...\n", i);
                    exit(1);
                }
            }

            for (i = 0; i < NUM_THREADS; ++i)
            {
                if (pthread_join(threads[i], NULL))
                {
                    printf("Unable to join thread %d, exiting...\n", i);
                    exit(1);
                }
            }
        }
        printf("Test: Check if %s algorithm provides mutual exclusion with %d threads: ", name, NUM_THREADS);

        if (privacy_violated)
        {
            printf("FAILED!!\n");
            return 1;
        }

        printf("OK\n");
    }
    return 0;
}
