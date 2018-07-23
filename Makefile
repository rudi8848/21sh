all: pipe.c
	gcc -o test -g3 pipe.c

clean:
	rm test
