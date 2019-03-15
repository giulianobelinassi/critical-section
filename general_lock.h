#ifndef LOCK_H
#define LOCK_H

/*This file generalizes a lock. A lock can either be initalized
 *using the bakery algorithm or with the gate algorithm
 */

typedef enum {
    LOCK_BAKERY,
    LOCK_GATE
} lock_alg_t;


/*Initialize the lock using algorithm described in algorithm
 *and num_threads threads.
 *Return 0 if success, 2 if number_of_threads is bigger
 *than MAX_PROCESSES, or 1 if unknown algorithm.
 */
int lock_init(int num_threads, lock_alg_t algorithm);

/*Try to enter in the critical section. If unable to, await until it
 *is available.
 */
void lock_lock(int thread_id);

/*Leaves critical section*/
void lock_unlock(int thread_id);

/*Get current initialized algorithm*/
lock_alg_t lock_get_current_algorithm(void);

/*returns array with all algorithms*/
const lock_alg_t* lock_get_all_algorithms(void);

/*return the name of the algorithm alg*/
const char* lock_get_algorithm_name(lock_alg_t alg);

/*return the name of the current algorithm being used*/
const char* lock_get_current_algorithm_name();

/*return the number of elements that the array containing all algorithms have*/
int lock_get_number_of_algorithms();

#endif /*LOCK_H*/
