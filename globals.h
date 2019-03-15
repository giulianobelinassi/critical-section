#ifndef GLOBALS_H
#define GLOBALS_H

#include <sched.h>

#define MAX_PROCESSES 32768
#define GLOBALS_AWAIT(x) while (!(x)) sched_yield()

#define STATIC_LEN(x) (sizeof(x)/sizeof(*x))

#endif /*GLOBALS_H*/
