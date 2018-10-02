PROGRAM = producer-consumer1
OBJECTS = ll_double.o threads.o synchronization.o producer-consumer1.o

test-clean: clean test

test: $(OBJECTS)
	clang -g -o test $(OBJECTS) -I.

%.o: %.c
	clang -g -c -o $@ -I. -Inet $<

clean:
	rm -f *.o test
