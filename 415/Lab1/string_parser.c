/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
	// rather than use the character buffer directly, make temporary copies
	// please see https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/ for referenced code
	int count = 0;
	char* token;
	char* rest = buf;

	// null check b/c while loop isn't giving accurate count yet

	// || (buf[0] == '\0')
	//return 0;
	if(strlen(buf) == 0) {
		printf("%s\n", "lack of buf");
		return count;
		//return 0;
	//	return 0;
	} else if (buf[0] == '\0') {
		printf("%s\n", "null character found");
		count++;
	} else {
		//iterate
		while (token = strtok_r(rest, delim, &rest)) {
			count++;
			//printf("%s\n", token);
			// this may only be counting delims - adjust if necessary (null check it) ^ see above about null at last token
			}
		}
		printf("%d\n", count);
		return count;
	}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/
	command_line var;
	var.num_token = count_token(buf, delim);;
	char* token;
	char* rest = buf;
	int i = 0;
	//num_token = count_token(buf, delim);
	var.command_list = malloc(var.num_token * sizeof(char*));
	//for (int i = 0, i < num_token, i++) {
	while (token = strtok_r(rest, delim, &rest)) {
		var.command_list[i] = malloc(strlen(token) * sizeof(char));
		var.command_list[i] = token;
		i++;
	}
	var.command_list[i] = NULL;
	return var;
	//}
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
	//for (int i = 0; i <= command->num_token; i++) {
	//	free(command->command_list[i]);
	//}
	free(command->command_list);
	
}
