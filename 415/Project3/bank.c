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
void printBalance(account *acct_ary);
void process_transaction(char** commandArg);
void update_balance();

account *acct_ary;
int ctr;

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
    ctr = 0;

    //account *acct_ary;
    
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
            getline(&line, &len, fp);
            strcpy(acct_ary[i].account_number, line);
            acct_ary[i].account_number[strcspn(acct_ary[i].account_number,"\n")] = '\0';
            getline(&line, &len, fp);
            strcpy(acct_ary[i].password, line);
            acct_ary[i].password[strcspn(acct_ary[i].password,"\n")] = '\0';
            getline(&line, &len, fp);
            acct_ary[i].balance = atof(line);
            getline(&line, &len, fp);
            acct_ary[i].reward_rate = atof(line);
        }
        //printAccounts(acct_ary);
        // accounts filled, begin processes...
        
        while ((read = getline(&line, &len, fp)) != -1) {
            //printf("Retrieved line of length %zu:\n", read);
            //printf("%s", line);
            process_transaction(&line);
            //ctr++;
            if (ctr % 5000){
                update_balance();
            }
        }
        update_balance();
        
        fclose(fp);
        if (line)
            free(line);
    }
    free(filenameSrc);
    //for debugging
    printBalance(acct_ary);
    //printAccounts(acct_ary);
    return 0;

}

void printAccounts(account *acct_ary)
{
    printf("\nNumber of Accounts: %d\n",numAcct);
    for (int i = 0; i < numAcct; i++) {
        printf("\nAccount Number: %s\n",acct_ary[i].account_number);
        printf("Password: %s\n",acct_ary[i].password);
        printf("Balance: %f\n",acct_ary[i].balance);
        printf("Reward Rate: %f\n",acct_ary[i].reward_rate);
        printf("Tracker: %f\n",acct_ary[i].transaction_tracter);
    }
    return;
}

void printBalance(account *acct_ary)
{
    for (int i = 0; i < numAcct; i++) {
        printf("%d balance:\t%.2f\n\n",i,acct_ary[i].balance);
    }
    return;
}

void process_transaction(char** commandArg){
    command_line token_buffer;
    token_buffer = str_filler(*commandArg, " ");
    //double actNum = atof(token_buffer.command_list[1]);
    //double actPass = atof(token_buffer.command_list[2]);
    for (int i = 0; i < numAcct; i++) {
        if (acct_ary[i].account_number == token_buffer.command_list[1]){
            if (acct_ary[i].password == token_buffer.command_list[2]){
                if (strcmp("C",token_buffer.command_list[0]) == 0) {
                    printf("Current Balance: %f",acct_ary[i].balance);
                } else if (strcmp("D",token_buffer.command_list[0]) == 0) {
                    double amount = atof(token_buffer.command_list[3]);
                    acct_ary[i].transaction_tracter += amount;
                    ctr++;
                } else if (strcmp("W",token_buffer.command_list[0]) == 0) {
                    double amount = atof(token_buffer.command_list[3]);
                    acct_ary[i].transaction_tracter -= amount;
                    ctr++;
                } else if (strcmp("T",token_buffer.command_list[0]) == 0) {
                    ctr++;
                    //double dest = atof(token_buffer.command_list[3]);
                    double amount = atof(token_buffer.command_list[4]);
                    for (int j = 0; j < numAcct; j++) {
                        if (acct_ary[j].account_number == token_buffer.command_list[3]){
                            acct_ary[i].transaction_tracter -= amount;
                            acct_ary[j].transaction_tracter += amount;
                            break;
                        }
                    }
                } else {
                    printf("Error - Command Unrecognized, Failed to Process: %s\n",*commandArg);
                }
            }
            break;
        }
    }
    free_command_line (&token_buffer);
	memset (&token_buffer, 0, 0);
    return;
    
    // parse argument as a command, tokenizing it. Might do this as the argument instead
    // make a switch, or something similar to previous project, where we check the type and take appropriate action
    // D = Deposit, [Code] [AcctNum] [Pass] [Credit], Add Credit to Acct's Tracker (acct_ary[x].account_number == AcctNum)
    // W = Withdrawal, same codes, Subtract ||
    // T = Transfer [Code] [SourceAcct] [Pass] [DestAcct] [Credit], Remove from Source Tracker, Add to Dest
    // C = Check Balance, [Code] [SourceAcct] [Pass], print current balance prior to initiating update
}

void update_balance(){
    // this should apply to every account
    // "add the account balance to the transaction_tracter * reward_rate"
    // or should this reward rate instead be applied to every deposit / incoming transfer?
    for (int i = 0; i < numAcct; i++) {
        //acct_array[i].transaction_tracter += acct_ary[i].balance;
        acct_ary[i].balance += (acct_ary[i].transaction_tracter * acct_ary[i].reward_rate);
        acct_ary[i].transaction_tracter = 0;
        //double inter = acct_ary[i].balance * acct_ary[i].reward_rate;
        //acct_ary[i].balance = 
    }
    return;
}