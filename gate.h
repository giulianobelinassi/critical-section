#ifndef GATE_H
#define GATE_H

/*Initialize the gate algorithm internal structures.
 *Return 0 if success, 1 if number_of_threads is bigger
 *than MAX_PROCESSES.
 */
int gate_init(int number_of_threads);

/*Try to enter in the critical section. If unable to, await until it
 *is available.
 */
void gate_lock(int thread_id);

/*Leaves critical section*/
void gate_unlock(int thread_id);

#endif /*GATE_H*/
