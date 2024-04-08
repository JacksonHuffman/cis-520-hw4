#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4

#define LINE_COUNT 1000000

pthread_mutex_t mutex;
int line_max_ascii[LINE_COUNT];

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
    //char theChar;
    int i;
    //int local_char_count[ALPHABET_SIZE];

    int startPos = ((int) myID) * (LINE_COUNT / NUM_THREADS);
    int endPos = startPos + (LINE_COUNT / NUM_THREADS);

    printf("myID = %d startPos = %d endPos = %d \n", (int) myID, startPos, endPos);


    pthread_mutex_lock (&mutex);
    for (i = startPos; i < endPos; i++) {
        // get the line i
        line_max_ascii[i] = get_max_ascii_val(/*line i*/);
    }
    pthread_mutex_unlock (&mutex);

    pthread_exit(NULL);
}

main(int argc, char **argv) 
{
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

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

    
    pthread_mutex_destroy(mutex);
	printf("Main: program completed. Exiting.\n");
	pthread_exit(NULL);
}