# An Implementation of Critical Section Algorithms

Here I implemented some critical sections algorithms in order to show that they actually work.

## Compiling
Make sure you have `gcc` and `make` installed, then run
```
make
```
This will create a executable called `main`, which you can run.

## Running Tests
Just run
```
make test
```
If some of then fails, then report a bug.

## Linux Scheduler
I added code with regard to change the way the threads are scheduled by Linux. Check the code in `main.c`.

## Adding New Algorithms
Create a new `.c` and a `.h` file with an function to `init`, `lock` and `unlock` functions, like the ones
implemented in `gate.c` and `bakery.h`, then add code in `general_lock.c` and `general_lock.h` to register your algorithm.
