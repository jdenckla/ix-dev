#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <dirent.h>
// #include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include "MCP.h"


#define _GNU_SOURCE
#define SIZE 1024

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

	for (int i = 0; i < numLines; i++)
	{
		small_token_buffer = str_filler (comm_array[i], " ");
		
		pid_array[i] = fork();
		//pid_array[i] = 0;
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
			exit(-1);
		}
	}
	for (int c = 0; c < numLines; c++){
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