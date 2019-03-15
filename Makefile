CC      = gcc
CFLAGS  = -O3 -march=native -flto -g -Wall -pedantic -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
LDFLAGS = -lpthread -lm -fwhole-program


main: main.o bakery.o gate.o general_lock.o statistics.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: test
test: test.o bakery.o gate.o general_lock.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	./test

test.o: test.c bakery.c gate.c general_lock.c bakery.h gate.h general_lock.h globals.h
	$(CC) $(CFLAGS) -c $<

main.o: main.c bakery.c gate.c general_lock.c bakery.h gate.h general_lock.h globals.h statistics.c statistics.h
	$(CC) $(CFLAGS) -c $<

bakery.o: bakery.c bakery.h globals.h
	$(CC) $(CFLAGS) -c $<

gate.o: gate.c gate.h globals.h
	$(CC) $(CFLAGS) -c $<

general_lock.o: general_lock.c general_lock.h bakery.c bakery.h gate.c gate.h globals.h
	$(CC) $(CFLAGS) -c $<

statistics.o: statistics.c statistics.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o test main
