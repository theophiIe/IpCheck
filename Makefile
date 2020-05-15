CC = gcc
CFLAGS = -Wall -g -pthread

run: compile
	./main

compile: clean
	$(CC) $(CFLAGS) main.c -L. -liof -o main

valgrind: compile
	valgrind ./main

clean:
	rm -f main
	ls
