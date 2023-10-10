#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "string_parser.h"

#define _GNU_SOURCE

// Note - there should be a file mode if prog is started with '-f'


int main(int argc, char const *argv[])
{
	char userInput[255];
	// getopt ref: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
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

	
	
	do {
		printf(">>>:");
		scanf("%s", userInput);

		if (strcmp("exit",userInput) == 0) {
			break;
		} else if (strcmp("ls",userInput) == 0) {
			//listDir();
		} else if (strcmp("pwd",userInput) == 0) {
			//showCurrentDir();
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
	} while(1);

	return 0;
}