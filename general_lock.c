#include "general_lock.h"
#include "globals.h"

#include "bakery.h"
#include "gate.h"

static const lock_alg_t lock_all_algorithms[] = {LOCK_BAKERY, LOCK_GATE};
static const char* lock_algorithm_names[]     = {"bakery"   , "gate"};
static const int lock_num_algorithms = STATIC_LEN(lock_all_algorithms);

static lock_alg_t lock_current_algorithm;

static void (*lock)(int);
static void (*unlock)(int);

int lock_init(int num_threads, lock_alg_t algorithm)
{
    if (num_threads > MAX_PROCESSES)
        return 2;

    switch (algorithm)
    {
        case LOCK_BAKERY:
            bakery_init(num_threads);
            lock = &bakery_lock;
            unlock = &bakery_unlock;
            lock_current_algorithm = LOCK_BAKERY;
        break;

        case LOCK_GATE:
            gate_init(num_threads);
            lock = &gate_lock;
            unlock = &gate_unlock;
            lock_current_algorithm = LOCK_GATE;
        break;

        default:
            return 1; /*Unknown algorithm*/
        break;
    }
    return 0;
}

void lock_lock(int thread_id)
{
    (*lock)(thread_id);
}

void lock_unlock(int thread_id)
{
    (*unlock)(thread_id);
}

lock_alg_t lock_get_current_algorithm(void)
{
    return lock_current_algorithm;
}

const lock_alg_t* lock_get_all_algorithms(void)
{
    return lock_all_algorithms;
}

const char* lock_get_algorithm_name(lock_alg_t alg)
{
    return lock_algorithm_names[alg];
}

const char* lock_get_current_algorithm_name()
{
    return lock_algorithm_names[lock_current_algorithm];
}

int lock_get_number_of_algorithms()
{
    return lock_num_algorithms;
}
