CC = gcc
CFLAGS = -Wall -g -pthread

run: compile
	./main

compile: clean
	$(CC) $(CFLAGS) main.c -L. -liof -o main

valgrind: compile
	valgrind --leak-check=full -v ./main

clean:
	rm -f main
	rm -f *.txt
	ls
