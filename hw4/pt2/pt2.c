#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define NUM_THREADS 4
int NUM_THREADS;

#define ARRAY_SIZE 2000000
#define STRING_SIZE 16
#define ALPHABET_SIZE 26

char char_array[ARRAY_SIZE][STRING_SIZE];
int char_counts[ALPHABET_SIZE];			// global count of individual characters
int local_char_count[ALPHABET_SIZE];

int line_count = 0;

char getRandomChar()
{
	int randNum = 0;
	char randChar = ' ';

	randNum = ALPHABET_SIZE * (rand() / (RAND_MAX + 1.0)); 	// pick number 0 < # < 25
	randNum = randNum + 97;				// scale to 'a'
	randChar = (char) randNum;

	// printf("%c", randChar);
	return randChar;
}

int read_file(const char *file_name) {
    FILE *stream;
    line_count = 0;

    stream = fopen(file_name, "r");
    if (stream == NULL) {
        printf("ERROR: Unable to open %s\n", file_name);
        return -1;
    }

    ssize_t nread;
    char *line = NULL;
    size_t len = 0;
    while ((nread = getline(&line, &len, stream)) != -1) {
        memcpy(char_array[line_count], line, nread);
        line_count++;
    }

    fclose(stream);
    return 0;
}

void init_arrays()
{
  int i, j; 

  printf("Initializing arrays.\n"); fflush(stdout);

  for ( i = 0; i < ARRAY_SIZE; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
		 char_array[i][j] = getRandomChar();
	}
  }

  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
  	char_counts[i] = 0;
  }
}

void *count_array(void *rank)
{
  char theChar;
  int i, j, charLoc;
  int myID =  *((int*) rank);

  int startPos = ((long) myID) * (ARRAY_SIZE / NUM_THREADS);
  int endPos = startPos + (ARRAY_SIZE / NUM_THREADS);

  printf("myID = %d startPos = %d endPos = %d \n", myID, startPos, endPos); fflush(stdout);

					// init local count array
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
  	local_char_count[i] = 0;
  }
					// count up our section of the global array
  for ( i = startPos; i < endPos; i++) {
	for ( j = 0; j < STRING_SIZE; j++ ) {
	         theChar = char_array[i][j];
		 charLoc = ((int) theChar) - 97;
		 local_char_count[charLoc]++;
	}
  }
}

void print_results()
{
  int i,j, total = 0;

  					// then print out the totals
  for ( i = 0; i < ALPHABET_SIZE; i++ ) {
     total += char_counts[i];
     printf(" %c %d\n", (char) (i + 97), char_counts[i]);
  }
  printf("\nTotal characters:  %d\n", total); fflush(stdout);
}

main(int argc, char* argv[]) 
{
	int i, rc;
	int numtasks, rank;
	MPI_Status Status;

	const char* text_file_name = argv[1];

	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	NUM_THREADS = numtasks;
	printf("size = %d rank = %d\n", numtasks, rank);
	fflush(stdout);

	if ( rank == 0 ) {
		init_arrays();
        if (read_file(text_file_name) == -1) {
            exit(-1);
        }
	}

	MPI_Bcast(char_array, ARRAY_SIZE * STRING_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
		
	count_array(&rank);

	MPI_Reduce(local_char_count, char_counts, ALPHABET_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


	if ( rank == 0 ) {
		print_results();
	}

	MPI_Finalize();
	return 0;
}

