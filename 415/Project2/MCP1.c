#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
//#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

// line counter sourced from: https://stackoverflow.com/questions/30432856/best-way-to-get-number-of-lines-in-a-file-c
int countLines(char *filename) {
	int counter = 0;
	FILE *fp;
	char c;
	fp = fopen(filename,"r");
	if (fp == NULL) {
		perror("Error opening file");
		return -1;
	} else {
		while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
        	++counter;
	}
	flcose(fp);
	return counter;
}

int main(int argc, char const *argv[])
{
	// look at argc and argv to determine flag (strcmp) and file (attempt read), enforce <= 2 or 3 in argv
	// can probably use fopen in this case, but how to apply that to the getline below... 
	// likely read the file's lines as stdin, and/or freopen here
	FILE *inFile;
	FILE *freOp;
	char *filenameSrc;
	int flagSet = 0;
	char *exitText = "Bye Bye!";
	char *errCmd = "Error! Unrecognized command: ";
	char *errParam = "Error! Unrecognized parameters for command: ";

	char *errRedirOut;

	flagSet = 1;
	filenameSrc = strdup(argv[2]);
	int numLines = countLines(filenameSrc);
	/*
	freOp = freopen("output.txt","w",stdout);
	if (freOp == NULL) {
		char *errRedirOut = "Error! Failed to redirect output.";
		write(1,errRedirOut,strlen(errRedirOut));
		write(1,"\n",1);
		free(filenameSrc);
		return 1;
	}
	*/
	inFile = fopen(filenameSrc, "r");
	if (inFile == NULL) {
		char *errOpenInput = "Error! Failed to open input file.";
		write(1,errOpenInput,strlen(errOpenInput));
		write(1,"\n",1);
		free(filenameSrc);
		return 1;
	}
		
	
	do {
		size_t size = 1024;
		char *userInput = malloc (size);
		ssize_t chars_read;

		// Problem - we cannot alloc a process array without knowing number of proc
		// potential solution - count number of lines in a file, alloc a proc for each
		pid_t *pid_array;
		pid_array = (pid_t*)malloc(sizeof(pid_t) * numLines);

		command_line large_token_buffer;
		command_line small_token_buffer;
			
		chars_read = getline(&userInput, &size, inFile);
		
		if (chars_read < 0){
			char *EOFtext = "End of file";
			write(1,EOFtext,strlen(EOFtext));
			write(1,"\n",1);
			break;
		}
		//scanf("%s", userInput);
		large_token_buffer = str_filler (userInput, ";");
		for (int i = 0; i < numLines; i++)
		{
			if (large_token_buffer.command_list[i] != NULL) {
				small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");
				pid_array[i] = fork();
				if (pid_array[i] < 0)
				{
					//error handling
					perror("Forking Failed");
					exit(1);
				}
				if (pid_array[i] == 0)
				{
					path = small_token_buffer[0];
					arg = small_token_buffer[0] + 1;
					if (execvp (path, arg) == -1)
					{
						//error handling
						perror("iobound Failed");
						exit(1);
					}
					//
					exit(-1);
				}
			}		
			free_command_line(&small_token_buffer);
			memset (&small_token_buffer, 0, 0);
		}
		free_command_line (&large_token_buffer);
		memset (&large_token_buffer, 0, 0);
		free (userInput);
	} while(1);
	free(filenameSrc);
	write(1,exitText,strlen(exitText));
	write(1,"\n",1);
	return 0;
}