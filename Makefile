CC = gcc
EXEC = main
CFLAGS = -Wall -Wextra -Walloc-zero -g -pthread -O2
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all : clean compile
	./$(EXEC)

%.o : %.c
	$(CC) -o $@ -c $<

compile : $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $^

valgrind: compile
	valgrind --leak-check=full -v ./$(EXEC)

clean:
	rm -rf $(EXEC)
	rm -rf *.txt
	ls
