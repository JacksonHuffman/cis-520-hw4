#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_LINES 1000000
#define LINE_LENGTH 2500

char data[NUM_LINES][LINE_LENGTH];
int line_max_ascii[NUM_LINES];

double total_time;

int line_count = 0;

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

void process_lines(int start_pos, int end_pos) {
    for (int i = start_pos; i < end_pos; i++) {
        line_max_ascii[i] = get_max_ascii_val(data[i]);
    }
}

int main(int argc, char **argv) {
    int num_procs, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (argc < 2) {
        printf("Usage: %s <text file>\n", argv[0]);
        MPI_Finalize();
        exit(-1);
    }

    const char *text_file_name = argv[1];

    init_array();
    if (read_file(text_file_name) == -1) {
        MPI_Finalize();
        exit(-1);
    }

    int lines_per_proc = NUM_LINES / num_procs;
    int start_pos = my_rank * lines_per_proc;
    int end_pos = start_pos + lines_per_proc;

    if (my_rank == num_procs - 1) {
        end_pos += NUM_LINES % num_procs;
    }

    double time_start = MPI_Wtime();
    process_lines(start_pos, end_pos);
    double time_end = MPI_Wtime();

    total_time = time_end - time_start;

    printf("Process %d: Total time = %f seconds\n", my_rank, total_time);

    MPI_Finalize();
    return 0;
}