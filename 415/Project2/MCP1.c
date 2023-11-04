#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void signaler(pid_t* pid_ary, int size, int signal);

int main(int argc, char* argv[])
{
	
	// initialization of pid etc. like lab 4
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	} 

	// define n
	int n = atoi(argv[1]);
	// define process array
	//pid_t procArray[n];
	pid_t *procArray;
	procArray = (pid_t*)malloc(sizeof(pid_t) * n);

	char * arg[4];
	arg[0] = "./iobound";
	arg[1] = "-seconds";
	arg[2] = "5";
	arg[3] = NULL;

	int pid = 0;

	// initialize sigset
	sigset_t sigset;
	int sig;

	// create an empty sigset_t
	sigemptyset(&sigset);

	// use sigaddset() to add the SIGUSR1 signal to the set
	sigaddset(&sigset,SIGUSR1);

	// use sigprocmask() to add the signal set in the sigset for blocking
	sigprocmask(SIG_BLOCK,&sigset,NULL);
	

	for(int i = 0; i < n; i++)
	{
		procArray[i] = fork();
		pid = procArray[i];
		if (pid > 0) {
			// same fork as in lab 4
			;
		}

		if(pid == 0)
		{
			// print: Child Process: <pid> - Waiting for SIGUSR1…
			// wait for the signal
			printf("%s%d%s\n","Child Proccess: ",pid," - Waiting for SIGUSR1");
			//waitpid(procArray[pid],NULL,0);
			int signalInt = sigwait(&sigset,&sig);
			//sigwait(&sigset,&sig);
			//if (sig == SIGUSR1) {
			if (signalInt == 0) {
				printf("%s%d%s\n","Child Proccess: ",pid," - Received signal: SIGUSR1 - Calling exec()");
				if (execvp("./iobound", arg) == -1) {
					perror("iobound Failed");
					exit(1);
				}
			}
			
			// print: Child Process: <pid> - Received signal: SIGUSR1 - Calling exec().
			// call execvp with ./iobound like in lab 4
			
		}
		if (pid < 0) {
			// fork failed error
			perror("Forking Failed");
			exit(1);
		}
	}
	
	// send SIGUSR1 
	signaler(procArray,n,SIGUSR1);

	// send SIGSTOP 
	signaler(procArray,n,SIGSTOP);

	// send SIGCONT
	signaler(procArray,n,SIGCONT);

	// send SIGINT
	signaler(procArray,n,SIGINT);

	//script_print(procArray,n);
	//for (int j = 0; j < n; j++) {
	//	waitpid(procArray[j],NULL,0);
	//}

	free(procArray);

	return 0;
}

void signaler(pid_t* pid_ary, int size, int signal)
{
	// sleep for three seconds
	pid_t pid;
	sleep(3);

	for(int i = 0; i < size; i++)
	{
		pid = pid_ary[i];
		//printf("In loop\n");
		//if (pid > 0) {
		//parent = pid;
		//printf("In parent\n");
		//} else if (pid == 0) {
		//printf("In child\n");
		//child = pid;
		// print: Parent process: <pid> - Sending signal: <signal> to child process: <pid>
		printf("%s%d%s%s%s%d\n","Parent proccess: ",getpid()," - Sending signal: ",strsignal(signal), " - to child process ",pid);
		// send the signal 
		kill(pid, signal);

		//}
		//kill(pid,signal);
	}
}


// Instructions: 
/*

Read from a file, each line is a command and its args

For each command, MCP must launch a separate process to run the command:
fork() the proc
an exec() variant to run it

Once all proc's are running, MCP should wait for each to terminate
using a wait() variant

After all proc's termainate, MCP must use exit()

*/

// Pseudo from project description

for (int i = 0; i < line_number; i++)
{
pid_array[i] = fork();
if (pid_array[i] < 0)
{
//error handling
}
if (pid_array[i] == 0)
{
if (execvp (path, arg) == -1)
{
error handling
}
//
exit(-1);
}
}
