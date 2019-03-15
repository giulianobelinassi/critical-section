#define __GNU_SOURCE
#include "globals.h"
#include "general_lock.h"
#include "statistics.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <sched.h>
#include <pthread.h>
#include <time.h>

#define QUANTUM 100
#define NUM_EXECUTIONS 30

/* This defines the scheduler that will be used to schedule the threads.
 * There are several possibilities for this value, as specified in
 * 'man sched', but here is some interesting ones:
 *
 *    SCHED_OTHER - The default OS scheduler. Usually this is the
 *  'Complete Fair Scheduler' in Linux, but maybe your distro replaced it by another
 *  algorithm, like the 'MuQQS'. FreeBSD also sets a different scheduler.
 *    SCHED_FIFO  - A First In First Out scheduler. Requires sudo.
 *    SCHED_RR    - Round Robin scheduler. Like the FIFO, but defines a quantum
 *  for the threads and forces a context switch after this quantum elapses.
 *  Maybe this is the fairest algorithm for this problem. Requires sudo.
*/
#define OS_SCHEDULER SCHED_OTHER

static int64_t total_time = 0;
static int num_threads = 0;
static int num_accesses[MAX_PROCESSES];

static pthread_barrier_t barrier;

static uint64_t get_time(void)
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return ((uint64_t)(time.tv_sec)*1000000000 + (uint64_t)(time.tv_nsec));
}

void* consume_cpu(void* args)
{
    /*Ok if C and if pointer size > int size. For C++, set type to equal pointer size*/
    int my_id = (int) args;
    volatile int i;

    pthread_barrier_wait(&barrier);

    while (total_time > 0)
    {
        lock_lock(my_id);
        {
            num_accesses[my_id]++; /*Count that I entered the CS.*/
            for (i = 0; i < QUANTUM; ++i)
                ; /*Consume CPU*/
            total_time -= i;
        }
        lock_unlock(my_id);
    }

    return NULL;
}


int main(int argc, char* argv[])
{
    static pthread_t threads[MAX_PROCESSES];
    const lock_alg_t* algorithms = lock_get_all_algorithms();
    int num_algorithms = lock_get_number_of_algorithms();

    uint64_t param_total_time;
    int i, j, k;

    if (argc != 3)
    {
        printf("Usage: %s <number_of_threads> <total_time>\n", argv[0]);
        return 3;
    }

    num_threads = atoi(argv[1]);
    param_total_time  = atoll(argv[2]);

    for (j = 0; j < num_algorithms; ++j)
    {
        lock_alg_t algorithm = algorithms[j];
        const char* name = lock_get_algorithm_name(algorithm);
        uint64_t time1, time2;

        for (k = 0; k < NUM_EXECUTIONS; ++k)
        {
            if (lock_init(num_threads, algorithm))
            {
                printf("So many threads that I panicked!\n");
                return 2;
            }

            memset(num_accesses, 0x00, num_threads*sizeof(int));
            total_time = param_total_time;

            if (pthread_barrier_init(&barrier, NULL, num_threads + 1))
            {
                printf("Unable to create barrier, exiting...\n");
                exit(1);
            }

            for (i = 0; i < num_threads; ++i)
            {
                int scheduler = OS_SCHEDULER;
                int priority = sched_get_priority_max(scheduler);
                struct sched_param param = {.sched_priority = priority};

                if (pthread_create(&threads[i], NULL, &consume_cpu, (void*) i))
                {
                    printf("Unable to create thread %d, exiting...\n", i);
                    exit(1);
                }
                if (pthread_setschedparam(threads[i], scheduler, &param))
                    printf("WARNING: Unable to change the thread scheduling policy!\n");
            }

            pthread_barrier_wait(&barrier);

            time1 = get_time();
            for (i = 0; i < num_threads; ++i)
            {
                if (pthread_join(threads[i], NULL))
                {
                    printf("Unable to join thread %d, exiting...\n", i);
                    exit(1);
                }
            }
            time2 = get_time();

            if (pthread_barrier_destroy(&barrier))
            {
                printf("Unable to destroy the barrier, exiting...\n");
                exit(1);
            }

            printf("---------------------\n");
            printf("Algorithm: %s, Execution number: %d\n", name, k+1);
            printf("Elapsed time (in nanoseconds): %lu\n", (time2-time1));
            statistics_print(num_threads, num_accesses);
            printf("---------------------\n");
        }
    }
    return 0;
}
