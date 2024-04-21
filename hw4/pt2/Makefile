PROGRAM = hw4
CC = mpicc
CFLAGS=-mcmodel=medium
EXE = hw4
CODE = pt2.c

OBJECTS=$(CODE:.c=.o)

default: all

all: $(PROGRAM) 

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXE)
clean: 
	rm -f *.o *.exe *.out  *stackdump $(PROGRAM)