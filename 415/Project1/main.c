#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

// Note - there should be a file mode if prog is started with '-f'


int main(int argc, char const *argv[])
{
	
	// getopt ref: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
	
	/*
	int flag = 0;
	char *fname = NULL;
	int opt;
	//opterr = 0;

	while ((opt = getopt (argc, argv, "f:")) != -1) 
	switch (opt)
	{
		case 'f':
		flag = 1;
		if (argc != 2)
		{
			printf ("Usage ./pseudo-shell -f filename\n");
			return 1;
		}
		fname = optarg;
		break;
		case '?':
		if (optopt == 'f')
		fprintf(stderr, "Option -%c requires a filename as an argument.\n",optopt);
		else {
			fprintf(stderr, "Unknown option/argument");
		}
	}
	*/

	
	
	
	
	do {
		size_t size = 128;
		char *userInput = malloc (size);
		ssize_t chars_read;

		//size_t len = 128;
		//char* line_buf = malloc (len);

		command_line large_token_buffer;
		command_line small_token_buffer;
		printf(">>>:");
		// pos have file pass to stdin, or swap stdin stream for variable set by flag
		chars_read = getline(&userInput, &size, stdin);
		userInput[strcspn(userInput, "\r\n")] = 0;
		if (chars_read < 0){
			puts("Err - no input. Exiting.");
			free(userInput);
			return 1;
		}
		//scanf("%s", userInput);
		large_token_buffer = str_filler (userInput, ";");
		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++)
		{
			//printf ("\tLine segment %d:\n", i + 1);
			//tokenize large buffer
			//smaller token is seperated by " "(space bar)
			small_token_buffer = str_filler (large_token_buffer.command_list[i], " ");
			//iterate through each smaller token to print

			if (strcmp("exit",small_token_buffer.command_list[0]) == 0) {
				return 0;
			} else if (strcmp("ls",small_token_buffer.command_list[0]) == 0) {
				if (small_token_buffer.command_list[1] != NULL) {
					printf("%s\n", "Error - ls takes no additional parameters.");
				} else {
					listDir();
				}
			} else if (strcmp("pwd",small_token_buffer.command_list[0]) == 0) {
				if (small_token_buffer.command_list[1] != NULL) {
					printf("%s\n", "Error - pwd takes no additional parameters.");
				} else {
					showCurrentDir();
				}
			} else if (strcmp("mkdir",small_token_buffer.command_list[0]) == 0) {
				//makeDir();
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					printf("%s\n", "Error - mkdir takes one additional parameter.");
				} else {
					makeDir(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cd",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					printf("%s\n", "Error - cd takes one additional parameter.");
				} else {
					changeDir(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cp",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] == NULL) || (small_token_buffer.command_list[3] != NULL)) {
					printf("%s\n", "Error - cp takes two additional parameters.");
				} else {
					copyFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
				}
			} else if (strcmp("mv",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] == NULL) || (small_token_buffer.command_list[3] != NULL)) {
					printf("%s\n", "Error - mv takes two additional parameters.");
				} else {
					moveFile(small_token_buffer.command_list[1], small_token_buffer.command_list[2]);
				}
			} else if (strcmp("rm",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					printf("%s\n", "Error - rm takes one additional parameter.");
				} else {
					deleteFile(small_token_buffer.command_list[1]);
				}
			} else if (strcmp("cat",small_token_buffer.command_list[0]) == 0) {
				if ((small_token_buffer.command_list[1] == NULL) || (small_token_buffer.command_list[2] != NULL)) {
					printf("%s\n", "Error - cat takes one additional parameter.");
				} else {
					displayFile(small_token_buffer.command_list[1]);
				}
			} else {
				printf("%s\n","Error: Unrecognized command!");
				// print user input as well
			}

			//for (int j = 0; small_token_buffer.command_list[j] != NULL; j++)
			//{
				//printf ("\t\tToken %d: %s\n", j + 1, small_token_buffer.command_list[j]);
				// here we can interpret each part of a command (based on spaces)...
			//}


			//free smaller tokens and reset variable
			free_command_line(&small_token_buffer);
			memset (&small_token_buffer, 0, 0);
		}
		free_command_line (&large_token_buffer);
		memset (&large_token_buffer, 0, 0);
		free (userInput);

		/*
		if (strcmp("exit",userInput) == 0) {
			break;
		} else if (strcmp("ls",userInput) == 0) {
			listDir();
		} else if (strcmp("pwd",userInput) == 0) {
			showCurrentDir();
		} else if (strcmp("mkdir",userInput) == 0) {
			//makeDir();
		} else if (strcmp("cd",userInput) == 0) {
			//changeDir();
		} else if (strcmp("cp",userInput) == 0) {
			//copyFile();
		} else if (strcmp("mv",userInput) == 0) {
			//moveFile();
		} else if (strcmp("rm",userInput) == 0) {
			//deleteFile();
		} else if (strcmp("cat",userInput) == 0) {
			//displayFile();
		} else {
			printf("%s\n","Error: Unrecognized command!");
			// print user input as well
		}
		*/
	} while(1);

	return 0;
}