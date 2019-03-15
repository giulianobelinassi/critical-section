#include "gate.h"
#include "globals.h"

#include <string.h>
#include <stdbool.h>

#define UNMARKED_GATE -1

volatile static int gate1;
volatile static int gate2;

static bool want[MAX_PROCESSES];
static int  N;

int gate_init(int n)
{
    if (n > MAX_PROCESSES)
        return 1;

    N = n;
    gate1 = gate2 = UNMARKED_GATE;
    memset(want, 0x00, n*sizeof(bool));

    return 0;
}

void gate_lock(int this_thread)
{
    int that_thread;

    for (;;)
    {
        gate1 = this_thread;
        want[this_thread] = true;
        __sync_synchronize();

        if (gate2 != UNMARKED_GATE)
        {
            want[this_thread] = false;
            __sync_synchronize();
            sched_yield();
            continue;
        }

        gate2 = this_thread;
        __sync_synchronize();

        if (gate1 != this_thread)
        {
            want[this_thread] = false;
            __sync_synchronize();

            for (that_thread = 0; that_thread < N; ++that_thread)
            {
                if (this_thread != that_thread)
                    GLOBALS_AWAIT(want[that_thread] == false);
            }

            if (gate2 != this_thread)
            {
                sched_yield();
                continue;
            }
            else
                want[this_thread] = true;
        }
        break;
    }
}

void gate_unlock(int this_thread)
{
    gate2 = UNMARKED_GATE;
    want[this_thread] = false;
}
