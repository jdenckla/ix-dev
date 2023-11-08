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

#ifndef STRING_PARSER_H_
#define STRING_PARSER_H_


#define _GUN_SOURCE


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


struct ProcArray {
	pid_t* array;
	int currentSize, maxSize, index;
} ProcArray; 

struct ProcArray* createProcArray(int maxSize) {
	struct ProcArray* procArray = (struct ProcArray*)malloc(sizeof(struct ProcArray));
	procArray->maxSize = maxSize;
	procArray->currentSize = procArray->index = 0;
	procArray->array = (pid_t*)malloc(maxSize * sizeof(pid_t));
	return procArray;
}

void destroyProcArray(struct ProcArray* procArray) {
	free(procArray);
	free(procArray->array);
	return;
}

int arrayIsFull(struct ProcArray* procArray){
	return (procArray->currentSize == procArray->maxSize);
}

int arrayIsEmpty(struct ProcArray* procArray){
	return (procArray->currentSize == 0);
}


int getIndex(struct ProcArray* procArray)
{
    if (arrayIsEmpty(procArray))
        return 0;
    return procArray->array[procArray->index];
}

int getCurrentSize(struct ProcArray* procArray)
{
    if (arrayIsEmpty(procArray))
        return 0;
    //return procArray->array[procArray->currentSize];
	return procArray->currentSize;
}

int getMaxSize(struct ProcArray* procArray)
{
    return procArray->array[procArray->maxSize];
}

void setMaxSize(struct ProcArray* procArray, int max)
{
	procArray->maxSize = max;
	return;
    //return procArray->array[procArray->maxSize];
}

void incrementCurrentSize(struct ProcArray* procArray)
{
	procArray->currentSize = (getCurrentSize(procArray) + 1);
	return;
    //return procArray->array[procArray->maxSize];
}

void addToProcArray(struct ProcArray* procArray, pid_t procID){
	if (arrayIsFull(procArray)){
		return;
	} else {
		// test to ensure PID isn't already in array!
		/*
		for (int i = 0; i < procArray->currentSize; i++){
			if (procArray->array[i] == procID) {
				printf("Process ID %d Already In ProcArray, Failed To Add!\n",procID);
				return;
			}
		}
		*/
		procArray->array[procArray->index] = procID;
		procArray->index = procArray->index + 1;
		//procArray->index = (procArray->index + 1) % procArray->maxSize;
		incrementCurrentSize(procArray);
		printf("Process %d Added to Process Array\n", procID);
	}
}


void printProcArray(struct ProcArray* procArray)
{
	printf("Printing Process Array\nCurrent Size: %d\nMax Size: %d\nActive Index: %d\n",procArray->currentSize, procArray->maxSize, procArray->index);
	if (arrayIsEmpty(procArray)) {
		printf("Error - Can't Print Empty Proc Array\n");
		return;
	} else {
		printf("Process Array Index | PID\n");
		for (int i = 0; i < procArray->currentSize; i++){
			printf("%d | %d\n", i, procArray->array[i]);
		}
		return;
	}
}
//
void removeFromProcArray(struct ProcArray* procArray, pid_t procID){
	if (arrayIsEmpty(procArray)){
		return;
	} else {
		for (int i = 0; i < procArray->currentSize; i++) {
			if (procArray->array[i] == procID) {
				printf("Process ID %d Found In ProcArray, Removing...\n",procID);
				for (int j = i; j < procArray->currentSize; j++) {
					if (j != (procArray->currentSize - 1)) {
						procArray->array[j] = procArray->array[j+1];
					} else {
						procArray->array[j] = -1;
						// garbage value now at end of array
					}
				}
				break;
			} else {
				printf("Process ID %d Not Found In ProcArray, Failed To Remove!\n",procID);
				return;
			}
		}
		procArray->currentSize = procArray->currentSize - 1;
		printf("Process %d Removed From Process Array\n", procID);
	}
}


#endif /* STRING_PARSER_H_ */
