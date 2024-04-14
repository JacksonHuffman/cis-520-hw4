#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4

#define NUM_LINES 1000000
#define LINE_LENGTH 2500

pthread_mutex_t mutex;          // mutex for line_max_ascii
char data[NUM_LINES][LINE_LENGTH];
int line_max_ascii[NUM_LINES];

int line_count = 0;
int line_count_per_thread = 0;
int line_left_over = 0;

void init_array()
{
    for (int i = 0; i < NUM_LINES; i++) {
        line_max_ascii[i] = 0;
    }
}

int read_file(const char* file_name)
{
    FILE *stream;
    line_count = 0;

    stream = fopen(file_name, "r");
    if (stream == NULL) {
        printf("ERROR; Unable to open %s\n", file_name);
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
    line_count_per_thread = (int)(line_count / NUM_THREADS);
    line_left_over = line_count % NUM_THREADS;
    return 0;
}

int get_max_ascii_val(char* line)
{
    int lineSize = strlen(line);

    int max_val = 0;

    for(int i = 0; i < lineSize; i++)
    {
        if((int)line[i] > max_val)
        {
            max_val = (int)line[i];
        }
    }
    
    return max_val;
}

void *process_line(void *myID)
{
    int local_line_max_ascii[line_count_per_thread];

    int startPos = ((int) myID) * line_count_per_thread;
    int endPos = startPos + line_count_per_thread;
    if ((int)myID == NUM_THREADS - 1) {
        endPos += line_left_over;
    }

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

    // Put the values in the global array
    pthread_mutex_lock (&mutex);
    for (int i = startPos; i < endPos; i++) {
        line_max_ascii[i] = local_line_max_ascii[i - startPos];
    }
    pthread_mutex_unlock (&mutex);

    pthread_exit(NULL);
}

void print_results() {
    for (int i = 0; i < line_count; i++) {
        printf("%i: %i\n", i, line_max_ascii[i]);
    }
}

main(int argc, char **argv) 
{
    if (argc < 2) {
        printf("%s <text file>\n", argv[0]);
        exit(-1);
    }

    const char* text_file_name = argv[1];

    init_array();
    if (read_file(text_file_name) == -1) {
        exit(-1);
    }

    printf("Line Count: %i, Line Count Per Thread: %i, Leftovers: %i\n", line_count, line_count_per_thread, line_left_over);

    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    /* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++ ) {
        int rc = pthread_create(&threads[i], &attr, process_line, (void *)i);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
	}

    /* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(int i=0; i<NUM_THREADS; i++) {
	    int rc = pthread_join(threads[i], &status);
	    if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
	    }
	}

    print_results();

    pthread_mutex_destroy(&mutex);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
}