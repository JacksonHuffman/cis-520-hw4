PROGRAM = hw4
CC = gcc
CFLAGS=-mcmodel=medium
EXE = hw4
CODE = mpi_imp.c

OBJECTS=$(CODE:.c=.o)

default: all

all: $(PROGRAM) 

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE)
clean: 
	rm -f *.o *.exe *.out  *stackdump $(PROGRAM)