#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
//#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE
#define SIZE 1024

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

int main(int argc, char const *argv[])
{
	//printf("Start the program\n");
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
		
	
	//do {
	size_t size = 1024;
	char *userInput = malloc (size);
	ssize_t chars_read;

	pid_t *pid_array;
	pid_array = (pid_t*)malloc(sizeof(pid_t) * numLines);

	command_line large_token_buffer;
	command_line small_token_buffer;
	
		
	chars_read = getline(&userInput, &size, inFile);
	
	if (chars_read < 0){
		return 0;
		//char *EOFtext = "End of file";
		//write(1,EOFtext,strlen(EOFtext));
		//write(1,"\n",1);
		//break;
	}
	large_token_buffer = str_filler (userInput, "\n");
	//printf("Reading lines...\n");
	for (int i = 0; i < numLines; i++)
	{
		//if (large_token_buffer.command_list[i] != NULL) {
		small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");
		//pid_array[i] = fork();
		pid_array[i] = 0;
		if (pid_array[i] < 0)
		{
			//error handling
			perror("Forking Failed");
			exit(1);
		}
		if (pid_array[i] == 0)
		{
			if (execvp (small_token_buffer.command_list[0], (small_token_buffer.command_list)) == -1)
			{
				//error handling
				perror("Execution Failed");
				exit(1);
			}
			//printf("A poor exit..\n");
			//exit(-1);
		}
		//}
	}
	free_command_line(&small_token_buffer);
	memset (&small_token_buffer, 0, 0);
	free_command_line (&large_token_buffer);
	memset (&large_token_buffer, 0, 0);
	free (userInput);
	//} while(1);
	free(filenameSrc);
	return 0;
}