#ifndef BAKERY_H
#define BAKERY_H

/*Initialize the bakery algorithm internal structures.
 *Return 0 if success, 1 if number_of_threads is bigger
 *than MAX_PROCESSES.
 */
int bakery_init(int number_of_threads);

/*Try to enter in the critical section. If unable to, await until it
 *is available.
 */
void bakery_lock(int thread_id);

/*Leaves critical section*/
void bakery_unlock(int thread_id);

#endif /*BAKERY_H*/
