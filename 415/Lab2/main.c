#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

#define _GNU_SOURCE


int main(int argc, char const *argv[])
{
	char userInput[255];
	char exit[255];
	//exit[0] = "exit";
	int exitBit = 1;
	
	
	do {
		printf(">>>:");
		scanf("%s", userInput);
		//test against command list
		//print error
		//else call function
		//exitBit == strcmp(userInput, exit);
		if (strcmp("exit",userInput) == 0) {
		//if (userInput == "exit") {
			break;
		} else if (strcmp("lfcat",userInput) == 0) {
			lfcat();
			return 0;
		} else {
			printf("%s\n","Error: Unrecognized command!");
		}
	} while(1);

	return 0;
}
