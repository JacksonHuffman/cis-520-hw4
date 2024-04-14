#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4

#define LINE_COUNT 1000000

pthread_mutex_t mutex;          // mutex for line_max_ascii
int line_max_ascii[LINE_COUNT];
const int line_count_per_thread = (int)(LINE_COUNT / NUM_THREADS);

void init_array()
{
    for (i = 0; i < LINE_COUNT; i++) {
        line_max_ascii[i] = 0;
    }
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

    printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);

    // init local line max ascii
    for (int i = 0; i < line_count_per_thread; i++) {
        local_line_max_ascii[i] = 0;
    }

    // Get the max ascii value for each line
    for (int i = startPos; i < endPos; i++) {
        // get the line i
        local_line_max_ascii[i - startPos] = get_max_ascii_val(/*line i*/);
    }

    // Put the values in the global array
    pthread_mutex_lock (&mutex);
    for (int i = startPos; i < endPos; i++) {
        // get the line i
        line_max_ascii[i] = local_line_max_ascii[i - startPos];
    }
    pthread_mutex_unlock (&mutex);

    pthread_exit(NULL);
}

main(int argc, char **argv) 
{
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    /* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < NUM_THREADS; i++ ) {
        rc = pthread_create(&threads[i], &attr, process_line, (void *)i);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
	}

    /* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS; i++) {
	    rc = pthread_join(threads[i], &status);
	    if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
	    }
	}

    // TODO: print results 0:99, 1:101, ...

    
    pthread_mutex_destroy(&mutex);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
}