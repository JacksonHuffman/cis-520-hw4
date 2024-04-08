
PROGRAM = hw4
CC = gcc
CFLAGS = -g -Wall
EXE = hw4
CODE = pthread_imp.c

OBJECTS=$(CODE:.c=.o)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE)
clean: 
	rm -f *.o *.exe *.out  *stackdump