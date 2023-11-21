#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 50

pthread_t thread_id[MAX_THREADS];    

//personal additional, see ref: https://courses.engr.illinois.edu/cs241/fa2010/ppt/10-pthread-examples.pdf
//int *tidPtr;

void * Print_thread(void* i)
{
    // TODO: Allocate the mutex lock
    pthread_mutex_lock(&mutex1);
    // TODO: print "First Print: " and the int i
    printf("First Print: %d\n",i);
    // TODO: sleep for one second
    sleep(1);
    // TODO: print "Second Print:" and the int i
    printf("Second Print: %d\n",i);
    // TODO: Release the mutex lock
    pthread_mutex_unlock(&mutex1);
    // TODO: You may also comment out the lock and unlock and see how the output changes

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

    // TODO: Create n threads, each calling Print_thread
	//int *tidPtr;
    for (i = 0; i < n; i++) {
        //tidPtr = (int *)malloc(sizeof(int));
        //*tidPtr = i;
        printf("Creating thread: %d\n",i);
        //rc = pthread_create(&thread_id[i], NULL, Print_thread, );
        rc = pthread_create(&thread_id[i], NULL, Print_thread, (void *)i);
        if (rc) {
            printf("Error - failed to create pthread: %d\n",rc);
            exit(-1);
        }
		//printf("Thread '%ld' created\n",&thread_id[i]);
    }
	for (i = 0; i < n; i++){
		pthread_join(thread_id[i], NULL);
	}
    // You may add print statements here as well if you want
    return 0;

}