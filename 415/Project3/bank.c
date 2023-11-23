#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "account.h"

#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>

#define _GNU_SOURCE
#define SIZE 1024

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 10

pthread_t thread_id[MAX_THREADS];

pid_t *pid_array;
int numLines;
int procIndex = 0;
int numProc;
int numAcct;

volatile sig_atomic_t got_interrupt = 0;

void handle_alarm(int sig);

// OG 50, alt max thread is 5000 and when we reach this threshold we pause

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

void printAccounts(account *acct_ary);

int main(int argc, char * argv[])
{
    if(argc < 2) 
    {
        printf("Error - Usage: ./bank inputfile.txt\n");
        exit(1); 
    }
    FILE *fp;
	char *filenameSrc;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    account *acct_ary;

    int ctr = 0;
    
	filenameSrc = strdup(argv[1]);
    numProc = countLines(filenameSrc);
    fp = fopen(filenameSrc, "r");
    if (fp == NULL) {
		char *errOpenInput = "Error! Failed to open input file.";
		write(1,errOpenInput,strlen(errOpenInput));
		write(1,"\n",1);
		free(filenameSrc);
		return 1;
    } else {
        getline(&line, &len, fp);
        numAcct = atoi(line);
        acct_ary = (account*)malloc(sizeof(account) * numAcct);
        for (int i = 0; i < numAcct; i++) {
            // using number of fields
            getline(&line, &len, fp);
            // discard index line
            printf("Passing garbo: %s\n",line);

            getline(&line, &len, fp);
            //acct_ary[i].account_number = line;
            printf("Passing acct: %s",line);
            strcpy(acct_ary[i].account_number, line);
            int len = strlen(acct_ary[i].account_number);
            if (acct_ary[i].account_number > 0 && acct_ary[i].account_number[len-1] == '\n') acct_ary[i].account_number[len-1] = '\0';
            printf("Now: %s\n",acct_ary[i].account_number);
            getline(&line, &len, fp);
            //acct_ary[i].password = line;
            printf("Passing pass: %s",line);
            strcpy(acct_ary[i].password, line);
            int leng = strlen(acct_ary[i].password);
            if (acct_ary[i].password > 0 && acct_ary[i].password[leng-1] == '\n') acct_ary[i].password[leng-1] = '\0';
            printf("Now: %s\n",acct_ary[i].password);

            getline(&line, &len, fp);
            printf("Passing bal: %s",line);
            acct_ary[i].balance = atof(line);
            printf("Now: %s\n",acct_ary[i].balance);

            getline(&line, &len, fp);
            printf("Passing trac: %s",line);
            acct_ary[i].transaction_tracter = atof(line);
            printf("Now: %s\n",acct_ary[i].transaction_tracter);
            // possibly sscanf(bal, "%lf", &acct_ary[i]->balance)
        }
        // accounts filled, begin processes...
        /*
        while ((read = getline(&line, &len, fp)) != -1) {
            //printf("Retrieved line of length %zu:\n", read);
            printf("%s", line);
            ctr++;
            if (ctr == numAcct){
                break;
            }
        }
        */
        fclose(fp);
        if (line)
            free(line);
    }
    //printAccounts(acct_ary);
    return 0;

}

void printAccounts(account *acct_ary)
{
    printf("\nNumber of Accounts: %d\n",numAcct);
    for (int i = 0; i < numAcct; i++) {
        // using number of fields
        printf("Account Number: %s\n",acct_ary[i].account_number);
        printf("Password: %s\n",acct_ary[i].password);
        printf("Balance: %f\n",acct_ary[i].balance);
        printf("Reward Rate: %f\n",acct_ary[i].reward_rate);
        printf("Tracker: %f\n",acct_ary[i].transaction_tracter);
    }
    return;
}