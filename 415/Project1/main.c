#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

// Note - there should be a file mode if prog is started with '-f'


int main(int argc, char const *argv[])
{
	// look at argc and argv to determine flag (strcmp) and file (attempt read), enforce <= 2 or 3 in argv
	// can probably use fopen in this case, but how to apply that to the getline below... 
	// likely read the file's lines as stdin, and/or freopen here
	FILE *inFile;
	FILE *freOp;
	char *filenameSrc;
	int flagSet = 0;
	char *exitText = "Goodbye!";

	char *errRedirOut;
	if (argc > 3) {
		char *errArgs1 = "Error - too many arguments given.";
		char *errArgs2 = "Usage: './pseudo-shell -f yourFilename.txt' or './pseudo-shell'";
		write(1,errArgs1,strlen(errArgs1));
        write(1,"\n",1);
        write(1,errArgs2,strlen(errArgs2));
        write(1,"\n",1);
		return 1;
	} else if (argc == 3) {
		if (strcmp(argv[1],"-f") == 0) {
			flagSet = 1;
			filenameSrc = strdup(argv[2]);
			freOp = freopen("output.txt","w",stdout);
			if (freOp == NULL) {
				char *errRedirOut = "Error - failed to redirect output.";
				write(1,errRedirOut,strlen(errRedirOut));
        		write(1,"\n",1);
				free(filenameSrc);
				return 1;
            }
			inFile = fopen(filenameSrc, "r");
			if (inFile == NULL) {
				char *errOpenInput = "Error - failed to open input file.";
				write(1,errOpenInput,strlen(errOpenInput));
        		write(1,"\n",1);
				free(filenameSrc);
				return 1;
			}
		} else {
			char *errFlag = "Error - incorrect argument flag provided: ";
			write(1,errFlag,strlen(errFlag));
			write(1,argv[1],strlen(argv[1]));
			write(1,"\n",1);
			char *errUsage = "Usage: './pseudo-shell -f yourFilename.txt' or './pseudo-shell'";
			write(1,errUsage,strlen(errUsage));
			write(1,"\n",1);
			return 1;
		}
	}
	do {
		size_t size = 1024;
		char *userInput = malloc (size);
		ssize_t chars_read;

		command_line large_token_buffer;
		command_line small_token_buffer;
		
		if (flagSet == 1) {		
			chars_read = getline(&userInput, &size, inFile);
		} else {
			write(1,">>>:",4);
			chars_read = getline(&userInput, &size, stdin);
			userInput[strcspn(userInput, "\r\n")] = 0;
		}
		if (chars_read < 0){
			if (flagSet == 1) {
				char *EOFtext = "End of file - exiting.";
				write(1,EOFtext,strlen(EOFtext));
				write(1,"\n",1);
				break;
			} else {
				char *errNoInput = "Error - no input. Exiting.";
				write(1,errNoInput,strlen(errNoInput));
				write(1,"\n",1);
				write(1,exitText,strlen(exitText));
				write(1,"\n",1);
				free(userInput);
				return 1;
			}
		}
		//scanf("%s", userInput);
		large_token_buffer = str_filler (userInput, ";");
		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
		{
			small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");

			if (strcmp("exit",small_token_buffer.command_list[0]) == 0) {
				free_command_line(&small_token_buffer);
				memset (&small_token_buffer, 0, 0);
				free_command_line (&large_token_buffer);
				memset (&large_token_buffer, 0, 0);
				free (userInput);
				if (flagSet == 1) {
					fclose(inFile);
					free(filenameSrc);
				}
				return 0;
			} else if (strcmp("ls",small_token_buffer.command_list[0]) == 0) {
				if (small_token_buffer.command_list[1] != NULL) {
					char *errLSparams = "Error - ls takes no additional parameters.";
					write(1,errLSparams,strlen(errLSparams));
					write(1,"\n",1);
				} else {
					listDir();
				}
			} else if (strcmp("pwd",small_token_buffer.command_list[0]) == 0) {
				if (small_token_buffer.command_list[1] != NULL) {
					char *errPWDparams = "Error - pwd takes no additional parameters.";
					write(1,errPWDparams,strlen(errPWDparams));
					write(1,"\n",1);
				} else {
					showCurrentDir();
				}
			} else if (strcmp("mkdir",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					char *errMKDIRparams = "Error - mkdir takes one additional parameter.";
					write(1,errMKDIRparams,strlen(errMKDIRparams));
					write(1,"\n",1);
				} else {
					makeDir(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cd",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					char *errCDparams = "Error - cd takes one additional parameter.";
					write(1,errCDparams,strlen(errCDparams));
					write(1,"\n",1);
				} else {
					changeDir(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cp",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] == NULL) || (small_token_buffer.command_list[3] != NULL)) {
					char *errCPparams = "Error - cp takes two additional parameters.";
					write(1,errCPparams,strlen(errCPparams));
					write(1,"\n",1);
				} else {
					copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
				}
			} else if (strcmp("mv",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] == NULL) || (small_token_buffer.command_list[3] != NULL)) {
					char *errMVparams = "Error - mv takes two additional parameters.";
					write(1,errMVparams,strlen(errMVparams));
					write(1,"\n",1);
				} else {
					moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
				}
			} else if (strcmp("rm",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					char *errRMparams = "Error - rm takes one additional parameter.";
					write(1,errRMparams,strlen(errRMparams));
					write(1,"\n",1);
				} else {
					deleteFile(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cat",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					char *errCATparams = "Error - cat takes one additional parameter.";
					write(1,errCATparams,strlen(errCATparams));
					write(1,"\n",1);
				} else {
					displayFile(small_token_buffer.command_list[1]);
				}
			} else {
				char *errUnkCmd = "Error - Unrecognized command: ";
				write(1,errUnkCmd,strlen(errUnkCmd));
				write(1,small_token_buffer.command_list[0],strlen(small_token_buffer.command_list[0]));
				write(1,"\n",1);
			}
			free_command_line(&small_token_buffer);
			memset (&small_token_buffer, 0, 0);
		}
		free_command_line (&large_token_buffer);
		memset (&large_token_buffer, 0, 0);
		free (userInput);
	} while(1);
	write(1,exitText,strlen(exitText));
	write(1,"\n",1);
	return 0;
}