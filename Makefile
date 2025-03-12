CFLAGS=-Wall -g

all:ex1 ex3 ex8 ex9 ex11


valgrind: ex4
	valgrind --track-origins=yes ./ex4

clean:
	rm -f ex1
	rm -f ex3
	rm -f ex4
	rm -f ex8
	rm -f ex9
	rm -rf ex11
