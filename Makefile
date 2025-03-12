CFLAGS=-Wall -g

all:ex1 ex3 ex8 ex9 ex11 ex17-2 ex17-7


valgrind: ex4 ex18-4
	valgrind --track-origins=yes ./ex4
	valgrind --track-origins=yes ./ex18-4

clean:
	rm -f ex1
	rm -f ex3
	rm -f ex4
	rm -f ex8
	rm -f ex9
	rm -f ex11
	rm -f ex17-2 ex17-7
