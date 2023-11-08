#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <dirent.h>
//#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include "MCP.h"


#define _GNU_SOURCE
#define SIZE 1024


pid_t *pid_array;
int numLines;
int procIndex = 0;

volatile sig_atomic_t got_interrupt = 0;

void handle_alarm(int sig);

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

void signaler(pid_t* pid_array, int size, int signal);

int main(int argc, char const *argv[])
{
	
	FILE *inFile;
	char *filenameSrc;

	filenameSrc = strdup(argv[1]);
	numLines = countLines(filenameSrc);
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
	procIndex = numLines - 1;
	
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

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	//printf("%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
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
		{	printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			printf("%s%d%s\n","Child Proccess: ",getpid()," - Waiting for SIGUSR1");
			int signalInt = sigwait(&sigset,&sig);
			if (signalInt == 0) {
				printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
				printf("%s%d%s\n","Child Proccess: ",getpid()," - Received signal: SIGUSR1 - Calling exec()");
				if (execvp (small_token_buffer.command_list[0], (small_token_buffer.command_list)) == -1)
				{
					//error handling
					perror("Execution Failed");
					exit(1);
				}
			}
		}
	}

	signaler(pid_array,numLines,SIGUSR1);

	signaler(pid_array,numLines,SIGSTOP);

	alarm(1);

	for (int c = 0; c < numLines; c++){
		// possibly alter to determine signal upon exit, (likely) using WIFEXITED and WEXITSTATUS: https://www.geeksforgeeks.org/exit-status-child-process-linux/
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

void signaler(pid_t* pid_array, int size, int signal)
{
	// sleep for one second
	pid_t pid;
	sleep(1);

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	//printf("%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	// the following anticipates wanting to send a signal to all processes. 
	for(int i = 0; i < size; i++)
	{
		pid = pid_array[i];
		printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		printf("%s%d%s%s%s%d\n","Parent proccess: ",getpid()," - Sending signal: ",strsignal(signal), " - to child process ",pid);
		// send the signal 
		kill(pid, signal);
	}
}

void handle_alarm( int sig ) {
	//got_interrupt = 1;
    //print_flag = true;
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	printf("Alarm Triggered, Stopping Proccess: %d\n",pid_array[procIndex]);
	kill(pid_array[procIndex], SIGSTOP);
	procIndex--;
	if (procIndex < 0) {
		procIndex = numLines - 1;
	}
	printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	printf("Continuing Proccess: %d\n",(pid_array[procIndex]));
	//kill(pid_array[procIndex], SIGCONT);
	pid_t killReturn = kill(pid_array[procIndex], SIGCONT);

	if (killReturn < 0) { 
		// no proc found
		if (errno = ESRCH){
			printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			printf("Failed to continue process, removing from queue...\n");
			// actually putting at end of queue and adjusting bounds, so as to avoid signalling an incorrect process...
			int a = pid_array[procIndex];
			for (int i = procIndex; i < numLines; i++) {
				pid_array[i] = pid_array[i+1];
			}
			pid_array[numLines] = a;
			numLines--;
			handle_alarm(1);
		} else {
			printf("%02d:%02d:%02d | ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
			printf("Failed to continue process, attempting next...\n");
			handle_alarm(1);
		}
	}
	alarm(1);
}