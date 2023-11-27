#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include "account.h"

//#include <fcntl.h>
//#include <sys/wait.h>
//#include <sys/types.h>
//#include <dirent.h>

#define _GNU_SOURCE
#define SIZE 1024

#define MAX_THREADS 10
#define debugText 0


// globals: thread array, account array, process counter, update counter (bank), number of accounts, number of lines in file (for debug)
// use shared memory for the following instead

pthread_t thread_id[MAX_THREADS];

account *acct_ary;
char ***process_queue;

int *processCounter;
int *updateCount;
int *numAcct;
int *numLines;

// helpers: tokenize strings, countlines, process input file, process queue (thread), process transaction (each line of queue), update balance (unique handler thread)

// predefine / declare helpers: 

// deprecate count_lines if unused

int count_token (char* buf, const char* delim);
command_line str_filler (char* buf, const char* delim);
void free_command_line(command_line *command);
int count_lines(char *filename);
void create_acct_outfiles(int i);
void parse_file(char *filename);
void outputBalance(account *acct_ary);
void *process_worker_queue(void *i);
void process_transaction(command_line token_buffer);
void update_balance();

// consider how we'll monitor the counter - an if within a while? should signal update thread, which pauses all workers, updates, then tells them to continue

// perhaps we could place the counter checks at each increment from within the process transaction function

int main(int argc, char * argv[])
{
    if(argc < 2) 
    {
        printf("Error - Usage: ./bank inputfile.txt\n");
        exit(1); 
    }

    int tid;

    struct stat st = {0};
    
    //printf("attempt mkdir output\n");
    const char *name = "output";
    mkdir(name,S_IRWXU);
    

    char *filename = strdup(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

	
    processCounter = malloc(sizeof(int) * 10000);
    numLines = malloc(sizeof(int) * 10000);
    updateCount = malloc(sizeof(int) * 10000);
    numAcct = malloc(sizeof(int) * 10000);
    if ((processCounter == NULL) || (numLines == NULL) || (updateCount == NULL) || (numAcct == NULL))
    {
        printf("Failed to alloc memory for initial counters (main)\n");
        return -1;
    }
	
	

    FILE *fp;

    int endOfFile = 0;
    int q = 0;
    //int max = *numLines;
    *numLines = count_lines(filename);
    //printf("Max at init: %d\n",*numLines);
    
    fp = fopen(filename, "r");
    getline(&line, &len, fp);
    //int numberOfAccounts = *numAcct;
    *numAcct = atoi(line);
    //printf("Num acct: %d\n",numAcct);
    //printf("%d Accounts\n",*numAcct);
	
    acct_ary = (account*)malloc(sizeof(account) * *numAcct);
    if (acct_ary == NULL) 
    {
        printf("Failed to alloc memory for account array\n");
        return -1;
    }
	
    fclose(fp);
    // number of threads to process requests
    process_queue = (char ***)malloc(sizeof(char**) * MAX_THREADS);
    if (acct_ary == NULL) 
    {
        printf("Failed to alloc memory for process queue (pointer)\n");
        return -1;
    }
    for (int z = 0; z < MAX_THREADS; z++)
    {
        // number of processes each thread could have to run
        process_queue[z] = (char **)malloc(sizeof(char*) * ((*numLines/MAX_THREADS) + 1));
        if (acct_ary == NULL) 
        {
            printf("Failed to alloc memory for process queue (threads)\n");
            return -1;
        }
        for (int y = 0; y < ((*numLines/MAX_THREADS) + 1); y++) 
        {
            // length of each process sentence
            process_queue[z][y] = (char *)malloc(sizeof(char) * 100);
            if (acct_ary == NULL) 
            {
                printf("Failed to alloc memory for process queue (sentences)\n");
                return -1;
            }
        }
    }
    parse_file(filename);
    free(filename);
    for (int a = 0; a < MAX_THREADS; a++)
    {
        int *worker = malloc(sizeof(int*));
        if (worker == NULL) 
        {
            printf("Failed to alloc memory for worker ID\n");
            return -1;
        }
        *worker = a;
        tid = pthread_create(&thread_id[a], NULL, process_worker_queue, worker);
        // anticipate each pausing from inside worker_queue
    }
    if (debugText == 1)
    {
        printf("Exited worker queue creation, awaiting completion\n");
    }
    // generate another thread for updating accounts? aka bank/manager thread
    for (int b = 0; b < MAX_THREADS; b++){
		pthread_join(thread_id[b], NULL);
	}
    //printf("Threads Complete - Attempt Balance Update From Main\n");
    // figure out how we're going to update accounts / monitor counter
    if (MAX_THREADS > 1)
    {
        update_balance();
    }
    //update_balance();
    //printf("Attempting Output\n");
    outputBalance(acct_ary);
    //int updateCounter = *updateCount;
    printf("Update Count: %d\n", *updateCount);
    for (int z = 0; z < MAX_THREADS; z++)
    {
        // number of processes each thread could have to run
        free(process_queue[z]);
    }
    //printf("Inner Queue Freed\n");
    free(process_queue);
    //printf("Outer Queue Freed\n");
    //free(processCounter);
    //printf("P Counter Freed\n");
    //free(updateCount);
    //printf("U Counter Freed\n");
    //free(numLines);
    //printf("L Counter Freed\n");
    //free(acct_ary);
    //printf("Acct Array Freed\n");
    return 0;
}

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
	while (token != NULL) 
    {
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
    if (var.command_list == NULL) 
        {
            printf("Failed to alloc memory for command_line parser\n");
            exit(0);
        }
	token = strtok_r(string, delim, &string);
	for (int j = 0; j < var.num_token; j++) 
    {
		var.command_list[j] = strdup(token);
		token = strtok_r(NULL, delim, &string);
	}
	var.command_list[var.num_token] = NULL;
	return var;
}

void free_command_line(command_line* command)
{	
	for (int i = 0; (i < command->num_token + 1); i++) 
    {
		free(command->command_list[i]);
	}
	free(command->command_list);
}

// count lines, credit to: https://codereview.stackexchange.com/questions/156477/c-program-to-count-number-of-lines-in-a-file
// deprecate if unused
int count_lines(char *filename)
{
	int counter = 0;
	FILE *fp;
	fp = fopen(filename,"r");
	char buffer[SIZE + 1], lastchar = '\n';
	size_t bytes;
	if (fp == NULL) 
    {
		perror("Error opening file");
		return -1;
	} else 
    {
		while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, fp))) 
        {
			lastchar = buffer[bytes - 1];
			for (char *c = buffer; (c = memchr(c, '\n', bytes - (c - buffer))); c++) 
            {
				counter++;
			}
		}
		if (lastchar != '\n') 
        {
			counter++; 
		}
	}
	fclose(fp);
	return counter;
}

void create_acct_outfiles(int i)
{
    char *filename;
    filename = strdup(acct_ary[i].out_file);
    FILE * afp = fopen(filename, "w+");
    if (afp == NULL) 
    {
        printf("Failed to open file to create!\n");
        free(filename);
        return;
    } else 
    {
        fprintf(afp,"account ");
        fprintf(afp,"%d",i);
        fprintf(afp,":\n");
    }
    fclose(afp);
    free(filename);
    return;
}

// read file, populate account array, create output file
void parse_file(char *file)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *filename = strdup(file);

    FILE *fp;

    int endOfFile = 0;
    int q = 0;

    //int numberOfAccounts = *numAcct;
    
    fp = fopen(filename, "r");
    if (fp == NULL) 
    {
		printf("Error! Failed to open output file for account creation.\n");
		free(filename);
		return;
    } else 
    {
        getline(&line, &len, fp);
        // pass number of accounts line
        //printf("Filling Data For %d Accounts\n",*numAcct);
        for (int i = 0; i < *numAcct; i++) 
        {
            acct_ary[i].transaction_tracter = 0;
            getline(&line, &len, fp);
            getline(&line, &len, fp);
            strcpy(acct_ary[i].account_number, line);
            acct_ary[i].account_number[strcspn(acct_ary[i].account_number,"\n")] = '\0';
            char iter[64];
            strcpy(iter,"output/");
            strcat(iter,acct_ary[i].account_number);
            strcat(iter,".txt");
            strcpy(acct_ary[i].out_file,iter);
            getline(&line, &len, fp);
            strcpy(acct_ary[i].password, line);
            acct_ary[i].password[strcspn(acct_ary[i].password,"\n")] = '\0';
            getline(&line, &len, fp);
            acct_ary[i].balance = atof(line);
            getline(&line, &len, fp);
            acct_ary[i].reward_rate = atof(line);
            int c;
            c = pthread_mutex_init(&acct_ary[i].ac_lock, NULL);
            // accounts populated, proceed to create output files
            create_acct_outfiles(i);
        }
        if (debugText == 1)
            {
                printf("Reading In File...\n");
            }
        while (endOfFile == 0) 
        {
            for (int a = 0; a < MAX_THREADS; a++) 
            {
                if ((read = getline(&line, &len, fp)) != -1) 
                {
                    strcpy(process_queue[a][q],line);
                    //potentially cleanup newline and null characters...
					//printf("%s added to queue\n",process_queue[a][q]);
					
                } else
                {
                    endOfFile = 1;
                    break;
                }
            }
            q++;
            if (endOfFile == 1)
            {
                break;
            }
        }
    }
    //printf("Q terminates at %d\n",q);
    //sleep(1);
    fclose(fp);
    free(filename);
    return;
}

void outputBalance(account *acct_ary)
{
    //printf("/////// Output Balance ///////\n");
    char *filename;
    filename = "output.txt";
    FILE * afp = fopen(filename, "w");
    //int numberOfAccounts = *numAcct;
    if (afp == NULL) 
    {
        char *errOpenInput = "Error! Failed to open input file.";
        write(1,errOpenInput,strlen(errOpenInput));
        write(1,"\n",1);
        //free(filename);
        return;
    } else 
    {
        for (int a = 0; a < *numAcct; a++){
            fprintf(afp,"%d",a);
            fprintf(afp," balance:\t");
            fprintf(afp,"%.2f\n\n",acct_ary[a].balance);
        }
    }
    fclose(afp);
    return;
}

// notice free of pointer here, may be useful elsewhere
void *process_worker_queue(void *i)
{
    int id = *((int *)i);
    //printf("Process Started For Worker: %d\n",id);
    // thread has started and been directed here. Tell it to pause and wait for signal.
    // upon signal, tokenize the next item in the workers queue.
    int job = 0;
    //int max = *numLines;
    //printf("Max at queue start: %d\n",*numLines);
    command_line token_buffer;
    //process_queue
    // using each worker's id (int i), grab sentence and tokenize it...
    //command_line token_buffer[100];
    int processing = 1;
    if (debugText == 1)
    {
        printf("Begin Worker Queue %d\n",id);
    }
    while (processing == 1)
    {
        //if (strlen(process_queue[id][job]) == 0)
        if (strcmp(process_queue[id][job],"") == 0)
        {
            processing = 0;
            break;
        } else
        {
            //printf("Attempting job: %d\n",job);
            token_buffer = str_filler(process_queue[id][job], " ");
            process_transaction(token_buffer);
            job++;
			//printf("Process Counter: %d\n",*processCounter);
            if (*processCounter >= 5000)
            {
				//printf("//////// Update Triggered by Process Count /////////\n");
                *processCounter = 0;
                update_balance();
            }
        } 
    }
    //printf("Process Queue Complete, Freeing Mem\n");
    //free_command_line (token_buffer);
	//memset (token_buffer, 0, 0);
    //printf("Process Queue Complete, Exiting Thread %d\n",id);
    free_command_line (&token_buffer);
	memset (&token_buffer, 0, 0);
    free(i);
    pthread_exit(NULL);
}

// for each process sentence, execute and update counter
void process_transaction(command_line token_buffer)
{
    // likely modify how these are passed
    //command_line *token_buffr = (command_line *)token_buf;
    //command_line token_buffer = *token_buffr;
    //int pctr = *processCounter;
    //int numberOfAccounts = *numAcct;
    #ifdef SYS_gettid
    pid_t tid = syscall(SYS_gettid);
    #else
    #error "SYS_gettid unavailable on this system"
    #endif
	if (debugText == 1)
	{
		// add timestamp here
		printf("Processing tid: %d",tid);
	}
    //printf("Against %d Accounts\n",*numAcct);
    for (int i = 0; i < *numAcct; i++) 
    {
        //printf("Compare acct: %s\n",acct_ary[i].account_number);
        //printf("Against buf.cmd: %s\n",token_buffer.command_list[1]);
        if (strcmp(acct_ary[i].account_number,token_buffer.command_list[1]) == 0)
        {
			if (debugText == 1)
			{
				printf(" - Accout Found");
			}
            if (strcmp(acct_ary[i].password,token_buffer.command_list[2]) == 0)
            {
				if (debugText == 1)
				{
					printf(" - Password Accepted");
				}
                if (strcmp("C",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1) 
					{
						printf(" - Check Balance\n");
					}
                    //printf("Check: %s\n",token_buffer.command_list[1]);
                } else if (strcmp("D",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1) 
					{
						printf(" - Deposit Accepted\n");
					}
                    double amount = atof(token_buffer.command_list[3]);
                    pthread_mutex_lock(&acct_ary[i].ac_lock);
                    //printf("Deposit: %s\n",token_buffer.command_list[1]);
                    acct_ary[i].balance += amount;
                    acct_ary[i].transaction_tracter += amount;
                    pthread_mutex_unlock(&acct_ary[i].ac_lock);
					*processCounter = *processCounter + 1;
                    //*processCounter++;
                } else if (strcmp("W",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1)
					{
						printf(" - Withdrawal Accepted\n");
					}
                    double amount = atof(token_buffer.command_list[3]);
                    pthread_mutex_lock(&acct_ary[i].ac_lock);
                    //printf("Withdraw: %s\n",token_buffer.command_list[1]);
                    acct_ary[i].balance -= amount;
                    acct_ary[i].transaction_tracter += amount;
                    pthread_mutex_unlock(&acct_ary[i].ac_lock);
					*processCounter = *processCounter + 1;
                    //*processCounter++;
                } else if (strcmp("T",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1)
					{
						printf(" - Transfer Initiated...");
					}
                    double amount = atof(token_buffer.command_list[4]);
                    for (int j = 0; j < *numAcct; j++) 
                    {
                        if (strcmp(acct_ary[j].account_number, token_buffer.command_list[3]) == 0)
                        {
                            pthread_mutex_lock(&acct_ary[i].ac_lock);
                            //printf("Transfer From: %s\n",token_buffer.command_list[1]);
                            acct_ary[i].balance -= amount;
                            acct_ary[i].transaction_tracter += amount;
                            pthread_mutex_unlock(&acct_ary[i].ac_lock);
                            pthread_mutex_lock(&acct_ary[j].ac_lock);
                            //printf("Transfer To: %s\n",token_buffer.command_list[3]);
                            acct_ary[j].balance += amount;
                            pthread_mutex_unlock(&acct_ary[j].ac_lock);
							*processCounter = *processCounter + 1;
                            //*processCounter++;
							if (debugText == 1)
							{
								printf(" Complete!\n");
							}
                        }
                    }
                } else 
                {
					if (debugText == 1)
					{
						printf(" - Command Not Recongized\n");
					}
                    break;
                }
            } else 
			{
				if (debugText == 1)
				{
					printf(" - Password Rejected\n");
				}
			}
            break;
        }
    }
    return;
}

void update_balance()
{
    //int updateCounter = *updateCount;
    //*updateCount++;
	*updateCount = *updateCount + 1;
    //printf("Update %d\n",*updateCount);
    //int numberOfAccounts = *numAcct;
    if (debugText == 1)
    {
        printf("Updating Balances... \n");
    }
    for (int i = 0; i < *numAcct; i++) 
    {
        pthread_mutex_lock(&acct_ary[i].ac_lock);
        double temp = (acct_ary[i].transaction_tracter * acct_ary[i].reward_rate);
        acct_ary[i].balance += temp;
        acct_ary[i].transaction_tracter = 0;
        char *filename;
        filename = strdup(acct_ary[i].out_file);
        FILE * afp = fopen(filename, "a");
        if (afp == NULL) 
        {
            printf("Failed to open file to update!\n");
            return;
        } else 
        {
            fprintf(afp,"Current Balance:\t");
            fprintf(afp,"%.2f\n",acct_ary[i].balance);
        }
        pthread_mutex_unlock(&acct_ary[i].ac_lock);
        fclose(afp);
        free(filename);
    }
	//printf("Done\n");
    return;
}
