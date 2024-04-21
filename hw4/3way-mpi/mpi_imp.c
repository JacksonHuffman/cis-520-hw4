#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int NUM_THREADS = 4;

#define NUM_LINES 1000000
#define LINE_LENGTH 2500

char data[NUM_LINES][LINE_LENGTH];
int line_max_ascii[NUM_LINES];
int local_line_max_ascii[NUM_LINES];

double total_time;

int line_count = 0;
int line_count_per_thread = 0;
int line_left_over = 0;

void init_array() {
    for (int i = 0; i < NUM_LINES; i++) {
        line_max_ascii[i] = 0;
    }
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
        memcpy(data[line_count], line, nread);
        line_count++;
    }

    fclose(stream);
    return 0;
}

int get_max_ascii_val(char *line) {
    int lineSize = strlen(line);
    int max_val = 0;

    for (int i = 0; i < lineSize; i++) {
        if ((int)line[i] > max_val) {
            max_val = (int)line[i];
        }
    }

    return max_val;
}

void process_lines(void *rank) {
    int local_line_max_ascii[line_count_per_thread];

    int myID =  *((int*) rank);

    int startPos = ((long) myID) * (NUM_LINES / NUM_THREADS);
    int endPos = startPos + (NUM_LINES / NUM_THREADS);

    printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);

    // init local array
    for (int i = 0; i < line_count_per_thread; i++) {
        local_line_max_ascii[i] = 0;
    }

    // Get the max ascii value for each line
    for (int i = startPos; i < endPos; i++) {
        // get the line i
        local_line_max_ascii[i - startPos] = get_max_ascii_val(data[i]);
    }
}

void print_results() {
    printf("line_count = %d\n", line_count);
    for (int i = 0; i < line_count; i++) {
        printf("%i: %i\n", i, line_max_ascii[i]);
    }
}

int main(int argc, char **argv) {
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
		init_array();
        if (read_file(text_file_name) == -1) {
            exit(-1);
        }
	}
	MPI_Bcast(data, NUM_LINES * LINE_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
		
	process_lines(&rank);

	MPI_Reduce(local_line_max_ascii, line_max_ascii, (NUM_LINES / NUM_THREADS), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if( rank == 0) {
        print_results();
    }
	
	MPI_Finalize();
	return 0;
}