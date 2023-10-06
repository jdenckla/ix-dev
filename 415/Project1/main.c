#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

#define _GNU_SOURCE

// Note - there should be a file mode if prog is started with '-f'


int main(int argc, char const *argv[])
{
	char userInput[255];
	
	do {
		printf(">>>:");
		scanf("%s", userInput);

		if (strcmp("exit",userInput) == 0) {
			break;
		} else if (strcmp("ls",userInput) == 0) {
			listDir();
		} else if (strcmp("pwd",userInput) == 0) {
			showCurrentDir();
		} else if (strcmp("mkdir",userInput) == 0) {
			makeDir();
		} else if (strcmp("cd",userInput) == 0) {
			changeDir();
		} else if (strcmp("cp",userInput) == 0) {
			copyFile();
		} else if (strcmp("mv",userInput) == 0) {
			moveFile();
		} else if (strcmp("rm",userInput) == 0) {
			deleteFile();
		} else if (strcmp("cat",userInput) == 0) {
			displayFile();
		} else {
			printf("%s\n","Error: Unrecognized command!");
			// print user input as well
		}
	} while(1);

	return 0;
}