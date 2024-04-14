
PROGRAM = hw4
CC = gcc
EXE = hw4
CODE = pthread_imp.c

OBJECTS=$(CODE:.c=.o)

default: all

all: $(PROGRAM) 

$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXE)
clean: 
	rm -f *.o *.exe *.out  *stackdump $(PROGRAM)