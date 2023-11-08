#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
// possibly verify the need for each of the above...
#include <string.h>
#include <signal.h>
//#include <bool.h>
#include <sys/wait.h>
#include "MCP.h"


#define _GNU_SOURCE
#define SIZE 1024

struct ProcArray {
	pid_t* array;
	int currentSize, maxSize, index;
} ProcArray; 

struct ProcArray* createProcArray(int maxSize) {
	struct ProcArray* procArray = (struct ProcArray*)malloc(sizeof(struct ProcArray));
	procArray->maxSize = maxSize;
	procArray->currentSize = procArray->index = 0;
	procArray->array = (pid_t*)malloc(maxSize * sizeof(pid_t));
	return procArray;
}

int arrayIsFull(struct ProcArray* procArray){
	return (procArray->currentSize == procArray->maxSize);
}

int arrayIsEmpty(struct ProcArray* procArray){
	return (procArray->currentSize == 0);
}

void addToProcArray(struct ProcArray* procArray, pid_t procID){
	if (arrayIsFull(procArray)){
		return;
	} else {
		// test to ensure PID isn't already in array!
		for (int i = 0; i < procArray->currentSize; i++){
			if (procArray->array[i] == procID) {
				printf("Process ID %d Already In ProcArray, Failed To Add!\n",procID);
				return;
			}
		}
		procArray->array[procArray->index] = procID;
		procArray->index = procArray->index + 1;
		//procArray->index = (procArray->index + 1) % procArray->maxSize;
		procArray->currentSize = procArray->currentSize + 1;
		printf("Process %d Added to Process Array\n", procID);
	}
}

int getIndex(struct ProcArray* procArray)
{
    if (arrayIsEmpty(procArray))
        return 0;
    return procArray->array[procArray->index];
}

int getCurrentSize(struct ProcArray* procArray)
{
    if (arrayIsEmpty(procArray))
        return 0;
    return procArray->array[procArray->currentSize];
}

int getMaxSize(struct ProcArray* procArray)
{
    return procArray->array[procArray->maxSize];
}

void setMaxSize(struct ProcArray* procArray, int max)
{
	procArray->maxSize = max;
	return;
    //return procArray->array[procArray->maxSize];
}
//
void removeFromProcArray(struct ProcArray* procArray, pid_t procID){
	if (arrayIsEmpty(procArray)){
		return;
	} else {
		for (int i = 0; i < procArray->currentSize; i++) {
			if (procArray->array[i] == procID) {
				printf("Process ID %d Found In ProcArray, Removing...\n",procID);
				for (int j = i; j < procArray->currentSize; j++) {
					if (j != (procArray->currentSize - 1)) {
						procArray->array[j] = procArray->array[j+1];
					} else {
						procArray->array[j] = -1;
						// garbage value now at end of array
					}
				}
				break;
			} else {
				printf("Process ID %d Not Found In ProcArray, Failed To Remove!\n",procID);
				return;
			}
		}
		procArray->currentSize = procArray->currentSize - 1;
		printf("Process %d Removed From Process Array\n", procID);
	}
}

// def global process array for future use...
struct ProcArray* procArray;

volatile sig_atomic_t got_interrupt = 0;

void handle_alarm( int sig ) {
	//got_interrupt = 1;
    //print_flag = true;
	//if (signal == SIGALRM) {
	printf("Alarm Triggered, Stopping Proccess: %d\n",procArray->array[getIndex(procArray)]);
	kill(getIndex(procArray), SIGSTOP);
	procArray->index = (getIndex(procArray) + 1) % getCurrentSize(procArray);
	//procArray->index = (procArray->index + 1) % procArray->currentSize;
	
	printf("Starting Proccess: %d\n",procArray->array[getIndex(procArray)]);
	kill(procArray->array[getIndex(procArray)], SIGCONT);
	
	// Attempt to continue process aka if (killReturn != 0)
	pid_t killReturn = kill(procArray->array[getIndex(procArray)], SIGCONT);
	if (killReturn) { 
		// no proc found
		removeFromProcArray(procArray, getIndex(procArray));
		//procArray->index = (procArray->index + 1) % procArray->currentSize;
		//signaler(pid_ary,size,SIGALRM);
	}
	
	//alarm(1);
	// potentially set this value to something greater..
	//}
}

int count_token (char* buf, const char* delim)
{
	char* save;
	strtok_r(buf,"\n",&save);
	char* string = strdup(buf);
	int count = 0;
	char* pointer;
	char* token;

	pointer = NULL;
	token = strtok_r(string, delim, &pointer);
	while (token != NULL) {
		token = strtok_r(NULL, delim, &pointer);
		count++;
	}
	free(string);
	return count;
}
	

command_line str_filler (char* buf, const char* delim)
{
	command_line var;
	var.num_token = count_token(buf, delim);
	char* token;
	char* string = buf;
	var.command_list = (char **)malloc((var.num_token + 1) * (sizeof(char*)));
	token = strtok_r(string, delim, &string);
	for (int j = 0; j < var.num_token; j++) {
		var.command_list[j] = strdup(token);
		token = strtok_r(NULL, delim, &string);
	}
	var.command_list[var.num_token] = NULL;
	return var;
}


void free_command_line(command_line* command)
{	
	for (int i = 0; (i < command->num_token + 1); i++) {
		free(command->command_list[i]);
	}
	free(command->command_list);
}


int countLines(char *filename)
{
// line counter sourced from: https://codereview.stackexchange.com/questions/156477/c-program-to-count-number-of-lines-in-a-file
	int counter = 0;
	FILE *fp;
	fp = fopen(filename,"r");
	char buffer[SIZE + 1], lastchar = '\n';
	size_t bytes;
	if (fp == NULL) {
		perror("Error opening file");
		return -1;
	} else {
		while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, fp))) {
			lastchar = buffer[bytes - 1];
			for (char *c = buffer; (c = memchr(c, '\n', bytes - (c - buffer))); c++) {
				counter++;
			}
		}
		if (lastchar != '\n') {
			counter++; 
		}
	}
	fclose(fp);
	return counter;
}

void signaler(pid_t* pid_ary, int size, int signal);

int main(int argc, char const *argv[])
{
	FILE *inFile;
	char *filenameSrc;

	filenameSrc = strdup(argv[1]);
	int numLines = countLines(filenameSrc);
	inFile = fopen(filenameSrc, "r");
	if (inFile == NULL) {
		char *errOpenInput = "Error! Failed to open input file.";
		write(1,errOpenInput,strlen(errOpenInput));
		write(1,"\n",1);
		free(filenameSrc);
		return 1;
	}
		
	
	size_t size = 1024;
	char *userInput = malloc (size);
	ssize_t read;
	printf("Break One\n");
	setMaxSize(procArray, numLines);
	printf("Break Two\n");
	pid_t *pid_array;
	pid_array = (pid_t*)malloc(sizeof(pid_t) * numLines);

	char **comm_array;
	comm_array = (char**)malloc(sizeof(char*) * numLines);

	char * line = NULL;
    size_t len = 0;
	int iter = 0;
	while ((read = getline(&line, &len, inFile)) != -1) {
		comm_array[iter] = strdup(line);
		iter++;
    }
	fclose(inFile);

	command_line small_token_buffer;

	int pid = 0;

	// initialize sigset
	sigset_t sigset;
	int sig;
	signal( SIGALRM, handle_alarm );

	// create an empty sigset_t
	sigemptyset(&sigset);

	// use sigaddset() to add the SIGUSR1 signal to the set
	sigaddset(&sigset,SIGUSR1);
	// do we need to do this with each type of signal?

	// use sigprocmask() to add the signal set in the sigset for blocking
	sigprocmask(SIG_BLOCK,&sigset,NULL);

	for (int i = 0; i < numLines; i++)
	{
		small_token_buffer = str_filler (comm_array[i], " ");
		pid_array[i] = fork();
		//pid_array[i] = 0;
		pid = pid_array[i];
		if (pid < 0)
		{
			//error handling
			perror("Forking Failed");
			exit(1);
		}
		if (pid == 0)
		{
			addToProcArray(procArray,getpid());
			printf("%s%d%s\n","Child Proccess: ",getpid()," - Added to Process Array");
			//printf("%s%d%s\n","Child Proccess: ",pid," - Waiting for SIGUSR1");
			printf("%s%d%s\n","Child Proccess: ",getpid()," - Waiting for SIGUSR1");
			int signalInt = sigwait(&sigset,&sig);
			if (signalInt == 0) {
				// possible place queue/dequeue here, based on SIGUSR1. We can use SIGSTOP/SIGCONT, but we need alarm somewhere too...
				//printf("%s%d%s\n","Child Proccess: ",getpid()," - Received signal: SIGUSR1 - Calling exec()");
				printf("%s%d%s\n","Child Proccess: ",getpid()," - Received signal: SIGCONT - Calling exec()");
				//alarm(1);
				if (execvp (small_token_buffer.command_list[0], (small_token_buffer.command_list)) == -1)
				{
					//error handling
					perror("Execution Failed");
					exit(1);
				}
			}
		}
	}

	// send SIGUSR1 
	// alternatively, place queue checking here. At this point, each child should know it's PID

	// send SIGSTOP 
	 // signaler(pid_array,numLines,SIGSTOP);

	signaler(pid_array,numLines,SIGUSR1);

	signaler(pid_array,numLines,SIGSTOP);

	alarm(1);

	// send SIGCONT
	// signaler(pid_array,numLines,SIGCONT);

	// send SIGINT
	// signaler(procArray,n,SIGINT);

	for (int c = 0; c < numLines; c++){
		// alter to determine signal upon exit, pos use WIFEXITED and WEXITSTATUS: https://www.geeksforgeeks.org/exit-status-child-process-linux/
		// see ref: https://stackoverflow.com/questions/47441871/why-should-we-check-wifexited-after-wait-in-order-to-kill-child-processes-in-lin
		// also, maybe https://stackoverflow.com/questions/60000733/how-to-check-if-all-child-processes-ended
		waitpid(pid_array[c],NULL,0);
	}
	for (int b = 0; b < numLines; b++){
		free(comm_array[b]);
	}
	free(comm_array);
	free_command_line(&small_token_buffer);
	memset (&small_token_buffer, 0, 0);
	free (userInput);
	free(filenameSrc);
	free(pid_array);
	exit(1);
	return 0;
}

void signaler(pid_t* pid_ary, int size, int signal)
{
	// sleep for one seconds
	pid_t pid;
	sleep(1);

	// the following anticipates wanting to send a signal to all processes. 
	
	/*
	if (signal == SIGINT) {
		printf("Interrupt triggered for proccess: %d\n",getpid());
		//alarm(1);
		// potentially set unique handler functionality...
	} else if (signal == SIGUSR1) {
		printf("SIGUSR1 triggered by proccess: %d, starting queue on %d...\n",getpid(),pid_ary[size-1]);
		kill(pid_ary[size-1], SIGUSR1);
		//alarm(1);
		// potentially set unique handler functionality...
	}
	*/
	/*
	if (signal == SIGCONT) {
		...
	}
	*/
	//else {
	// send to all processes
	for(int i = 0; i < size; i++)
	{
		pid = pid_ary[i];
		// print: Parent process: <pid> - Sending signal: <signal> to child process: <pid>
		printf("%s%d%s%s%s%d\n","Parent proccess: ",getpid()," - Sending signal: ",strsignal(signal), " - to child process ",pid);
		// send the signal 
		kill(pid, signal);
	}
	//}
}