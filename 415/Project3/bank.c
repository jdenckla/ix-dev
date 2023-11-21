#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "account.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 50

pthread_t thread_id[MAX_THREADS];    

//personal additional, see ref: https://courses.engr.illinois.edu/cs241/fa2010/ppt/10-pthread-examples.pdf

void * Print_thread(void* i)
{
    pthread_mutex_lock(&mutex1);
    printf("First Print: %d\n",i);
    sleep(1);
    printf("Second Print: %d\n",i);
    pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);

}

int main(int argc, char * argv[])
{
    int rc, i, n;

    if(argc < 2) 
    {
        printf("Please add the number of threads to the command line\n");
        exit(1); 
    }
    n = atoi(argv[1]);
    if(n > MAX_THREADS) n = MAX_THREADS;

    for (i = 0; i < n; i++) {
        printf("Creating thread: %d\n",i);
        rc = pthread_create(&thread_id[i], NULL, Print_thread, (void *)i);
        if (rc) {
            printf("Error - failed to create pthread: %d\n",rc);
            exit(-1);
        }
    }
	for (i = 0; i < n; i++){
		pthread_join(thread_id[i], NULL);
	}
    return 0;

}