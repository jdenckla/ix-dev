#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void script_print (pid_t* pid_ary, int size);

int main(int argc,char*argv[])
{
	if (argc != 2)
	{
		printf ("Wrong number of arguments\n");
		exit (0);
	} 
	
	if (typeof(argv[1]) != int)
	{
		printf ("Passed in argument must be of type int\n")
	}

	/*
	*	TODO
	*	#1	declare child process pool
	*	#2 	spawn n new processes
	*		first create the argument needed for the processes
	*		for example "./iobound -seconds 10"
	*	#3	call script_print
	*	#4	wait for children processes to finish
	*	#5	free any dynamic memory
	*/

	// define n
	int n = argv[1];
	// define process array
	pid_t procArray[n];
	int pid = 0;
	

	for(int i = 0; i < n; i++)
	{
		procArray[i] = fork();
		pid = procArray[i];
		if (pid > 0) {
			// same fork as in lab 4
			// child process
		}

		if(pid == 0)
		{
			// print: Child Process: <pid> - Waiting for SIGUSR1…
			// wait for the signal
			//printf("%s%d%s\n","Child Process: ", pid, " - Waiting for SIGURS1");
			//pause();
			// print: Child Process: <pid> - Received signal: SIGUSR1 - Calling exec().
			// call execvp with ./iobound like in lab 4
			execvp(./iobound -seconds 5);
			pause();
			script_print(procArray,i);

		}
		if (pid < 0) {
			// fork failed error
		}
	}
	// free memory

	return 0;
}


void script_print (pid_t* pid_ary, int size)
{
	FILE* fout;
	fout = fopen ("top_script.sh", "w");
	fprintf(fout, "#!/bin/bash\ntop");
	for (int i = 0; i < size; i++)
	{
		fprintf(fout, " -p %d", (int)(pid_ary[i]));
	}
	fprintf(fout, "\n");
	fclose (fout);

	char* top_arg[] = {"gnome-terminal", "--", "bash", "top_script.sh", NULL};
	pid_t top_pid;

	top_pid = fork();
	{
		if (top_pid == 0)
		{
			if(execvp(top_arg[0], top_arg) == -1)
			{
				perror ("top command: ");
			}
			exit(0);
		}
	}
}


