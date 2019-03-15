#include "bakery.h"
#include "globals.h"

#include <stdbool.h>
#include <string.h>

static int  number[MAX_PROCESSES];
static bool choosing[MAX_PROCESSES];
static int  N;

static int bakery_max(void)
{
    int i, max = -1;
    for (i = 0; i < N; ++i)
    {
        if (number[i] > max)
            max = number[i];
    }
    return max;
}

int bakery_init(int n)
{
    if (n > MAX_PROCESSES)
        return 1;

    N = n;
    memset(number, 0x00, n*sizeof(int));
    memset(choosing, 0x00, n*sizeof(bool));
    return 0;
}

void bakery_lock(int this_thread)
{
    int that_thread;

    choosing[this_thread] = true;
    __sync_synchronize();
    number[this_thread] = 1 + bakery_max();
    choosing[this_thread] = false;
    __sync_synchronize();

    for (that_thread = 0; that_thread < N; ++that_thread)
    {
        if (this_thread != that_thread)
        {
            GLOBALS_AWAIT(!choosing[that_thread]);

            GLOBALS_AWAIT (
                number[that_thread] == 0 ||
                number[this_thread] < number[that_thread] ||
                (number[this_thread] == number[that_thread] && this_thread < that_thread)
            );
        }
    }
}

void bakery_unlock(int this_thread)
{
    number[this_thread] = 0;
}
