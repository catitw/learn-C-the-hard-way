CFLAGS=-Wall -g

all:ex1 ex3


valgrind: ex4
	valgrind --track-origins=yes ./ex4

clean:
	rm -f ex1
	rm -f ex3
	rm -f ex4
