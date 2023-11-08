/*
 * string_parser.h, now MCP.h
 *
 *  Created on: Nov 8, 2020
 *      Author: gguan, Monil
 *  Modified by John Dencklau, as of Nov 5, 2023
 *
 *	Purpose: The goal of this dynamic helper string struct is to reliably 
 *			 tokenize strings base on different delimeter. Following this structure
 *           would help to keep the code clean.
 *
 */

#ifndef MCP_H_
#define MCP_H_


#define _GNU_SOURCE


typedef struct
{
    char** command_list;
    int num_token;
}command_line;

//this functions returns the number of tokens needed for the string array
//based on the delimeter
int count_token (char* buf, const char* delim);

//This functions can tokenize a string to token arrays base on a specified delimeter,
//it returns a struct variable
command_line str_filler (char* buf, const char* delim);


//this function safely free all the tokens within the array.
void free_command_line(command_line* command);


typdef struct
{
	pid_t* array;
	int currentSize, maxSize, index;
} ProcArray; 

struct ProcArray* createProcArray(int maxSize);

void destroyProcArray(struct ProcArray* procArray);

int arrayIsFull(struct ProcArray* procArray);

int arrayIsEmpty(struct ProcArray* procArray);

int getIndex(struct ProcArray* procArray);

int getCurrentSize(struct ProcArray* procArray);

int getMaxSize(struct ProcArray* procArray);

void setMaxSize(struct ProcArray* procArray, int max);

void incrementCurrentSize(struct ProcArray* procArray);

void addToProcArray(struct ProcArray* procArray, pid_t procID);

void printProcArray(struct ProcArray* procArray);

void removeFromProcArray(struct ProcArray* procArray, pid_t procID);

#endif /* MCP_H_ */
