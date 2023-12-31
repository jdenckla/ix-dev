#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include "account.h"
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <err.h>

#define _GNU_SOURCE
#define SIZE 4096

#define MAX_THREADS 10
#define processThreshold 5000
#define debugText 2


// globals: thread array, account array, process counter, update counter (bank), number of accounts, number of lines in file (for debug)
// use shared memory for the following instead

pthread_t thread_id[MAX_THREADS];
pthread_t monitorTID;
pthread_barrier_t startupBarrier;
pthread_cond_t working;
int *allowWork;
int *monitoring;

account *acct_ary;
account *save_ary;
char ***process_queue;


int *processCounter;
int *updateCount;
int *numAcct;
int *numLines;

pid_t *pid_array;
int *updateSavings;
int *killSavings;

// helpers: tokenize strings, countlines, process input file, process queue (thread), process transaction (each line of queue), update balance (unique handler thread)

// predefine / declare helpers: 

// deprecate count_lines if unused

int count_token (char* buf, const char* delim);
command_line str_filler (char* buf, const char* delim);
void free_command_line(command_line *command);
int count_lines(char *filename);
void create_acct_outfiles(int i);
void create_save_outfiles(int i);
void parse_file(char *filename);
void outputBalance(account *acct_ary);
void *monitor_transactions();
void *process_worker_queue(void *i);
void process_transaction(command_line *token_buffer);
void update_balance();
void update_savings();
void puddles();

// consider how we'll monitor the counter - an if within a while? should signal update thread, which pauses all workers, updates, then tells them to continue

// perhaps we could place the counter checks at each increment from within the process transaction function

int main(int argc, char * argv[])
{
    if(argc < 2) 
    {
        printf("Error - Usage: ./bank inputfile.txt\n");
        exit(1); 
    }

    /*
    int *processCounter;
    int *updateCount;
    int *numAcct;
    int *numLines;
    */
    
    int tid;

    struct stat st = {0};
    
    const char *name = "output";
    mkdir(name,S_IRWXU);
    const char *oname = "savings";
    mkdir(oname,S_IRWXU);

    char *filename = strdup(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    pthread_barrier_init(&startupBarrier, NULL, MAX_THREADS + 1);
	
    allowWork = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    monitoring = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    processCounter = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    numLines = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    updateCount = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    numAcct = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if ((processCounter == MAP_FAILED) || (numLines == MAP_FAILED) || (updateCount == MAP_FAILED) || (numAcct == MAP_FAILED))
    {
        printf("Failed to alloc memory for initial counters (main)\n");
        return -1;
    }
    updateSavings = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    killSavings = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if ((updateSavings == MAP_FAILED) || (killSavings == MAP_FAILED))
    {
        printf("Failed to alloc memory for process trackers\n");
        return -1;
    }
    *updateSavings = 0;
    *killSavings = 0;

    *monitoring = 1;
    *allowWork = 1;
    *processCounter = 0;
    *numLines = 0;
    *updateCount = 0;
    *numAcct = 0;
    
    FILE *fp;

    int endOfFile = 0;
    int q = 0;
    *numLines = count_lines(filename);
    
    fp = fopen(filename, "r");
    getline(&line, &len, fp);
    *numAcct = atoi(line);
	free(line);
	
    acct_ary = (account*)mmap(NULL, (sizeof(account) * *numAcct), PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //acct_ary = (account*)malloc(sizeof(account) * *numAcct);
    if (acct_ary == MAP_FAILED) 
    {
        printf("Failed to alloc memory for account array\n");
        return -1;
    }
    save_ary = (account*)mmap(NULL, (sizeof(account) * *numAcct), PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (save_ary == MAP_FAILED) 
    {
        printf("Failed to alloc memory for account array\n");
        return -1;
    }
	
    fclose(fp);

	int pid = 0;
    pid_array = (pid_t*)malloc(sizeof(pid_t) * 3);
    pid_array[1] = fork();
    pid = pid_array[1];

    if(pid == 0)
    {
        pid_array[0] = getpid();
        //printf("Puddles Savings Process Started\n");
        //printf("PID 0: %d\nPID 1: %d\n",pid_array[0],pid_array[1]);
        puddles();
        //printf("Puddles Savings Process Ended\n");
        return 1;
    }
    if (pid < 0) {
        // fork failed error
        perror("Forking Failed");
        exit(1);
    }
    //printf("PID 0: %d\nPID 1: %d\n",pid_array[0],pid_array[1]);
    kill(pid_array[1],SIGSTOP);

    process_queue = (char ***)malloc(sizeof(char**) * MAX_THREADS);
    if (process_queue == NULL) 
    {
        printf("Failed to alloc memory for process queue (pointer)\n");
        return -1;
    }
    for (int z = 0; z < MAX_THREADS; z++)
    {
        process_queue[z] = (char **)malloc(sizeof(char*) * ((*numLines/MAX_THREADS) + 1));
        if (process_queue[z] == NULL) 
        {
            printf("Failed to alloc memory for process queue (threads)\n");
            return -1;
        }
        
        for (int y = 0; y < ((*numLines/MAX_THREADS) + 1); y++) 
        {
            process_queue[z][y] = (char *)malloc(sizeof(char) * 100);
            if (process_queue[z][y] == NULL)
            {
                printf("Failed to alloc memory for process queue (sentences)\n");
                return -1;
            }
        }
        
    }
    parse_file(filename);
    free(filename);
    int mon;
    mon = pthread_create(&monitorTID,NULL,monitor_transactions,NULL);
    if (debugText > 0)
    {
        printf("Monitor Thread Created, Creating Workers...\n");
        //sleep(1);
    }
    for (int a = 0; a < MAX_THREADS; a++)
    {
        int *worker = malloc(sizeof(int*));
        if (worker == NULL) 
        {
            printf("Failed to alloc memory for worker thread\n");
            return -1;
        }
        *worker = a;
        tid = pthread_create(&thread_id[a], NULL, process_worker_queue, worker);
        // anticipate each pausing from inside worker_queue
    }
    if (debugText > 0)
    {
        printf("All Workers Created, Awaiting Barrier\n");
        //sleep(1);
    }

    pthread_barrier_wait(&startupBarrier);

    if (debugText > 0)
    {
        printf("Barrier Passed\n");
        //sleep(1);
    }

    
    
    // generate another thread for updating accounts? aka bank/manager thread
    for (int b = 0; b < MAX_THREADS; b++){
		pthread_join(thread_id[b], NULL);
	}
    if (debugText > 0)
    {
        printf("All Worker Threads Complete\n");
    }
    int updateRequired = 0;
    for (int c = 0; c < *numAcct; c++)
    {
        if (acct_ary[c].transaction_tracter != 0)
        {
            updateRequired = 1;
        }
    }
    if (updateRequired == 1)
    {
        update_balance();
    }
    sleep(1);
    *killSavings = 1;
    sched_yield();
    *monitoring = 0;

    outputBalance(acct_ary);
    sleep(1);
    printf("Update Count: %d\n", *updateCount);
    for (int z = 0; z < MAX_THREADS; z++)
    {
		for (int y = 0; y < ((*numLines/MAX_THREADS) + 1); y++) 
        {
            free(process_queue[z][y]);
        }
        free(process_queue[z]);
    }
    free(process_queue);
	free(pid_array);
    pthread_barrier_destroy(&startupBarrier);
    munmap(allowWork,SIZE);
    munmap(monitoring,SIZE);
    munmap(processCounter,SIZE);
    munmap(numLines,SIZE);
    munmap(updateCount,SIZE);
    munmap(numAcct,SIZE);
    //munmap(acct_ary,(sizeof(account) * *numAcct));
    //munmap(save_ary,(sizeof(account) * *numAcct));
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

void create_save_outfiles(int i)
{
    char *filename;
    filename = strdup(save_ary[i].out_file);
    FILE * afp = fopen(filename, "w+");
    if (afp == NULL) 
    {
        printf("Failed to open file to create!\n");
        free(filename);
        return;
    } else 
    {
        fprintf(afp,"account: ");
        fprintf(afp,"%d\n",i);
        fprintf(afp,"Current Savings Balance  ");
        fprintf(afp,"%.2f\n",save_ary[i].balance);
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
            save_ary[i].transaction_tracter = 0;
            getline(&line, &len, fp);
            getline(&line, &len, fp);
            strcpy(acct_ary[i].account_number, line);
            acct_ary[i].account_number[strcspn(acct_ary[i].account_number,"\n")] = '\0';
            memcpy(save_ary[i].account_number,acct_ary[i].account_number,strlen(acct_ary[i].account_number)+1);
            char iter[64];
            strcpy(iter,"output/");
            strcat(iter,acct_ary[i].account_number);
            strcat(iter,".txt");
            strcpy(acct_ary[i].out_file,iter);
            // differing directories
            char oter[64];
            strcpy(oter,"savings/");
            strcat(oter,save_ary[i].account_number);
            strcat(oter,".txt");
            strcpy(save_ary[i].out_file,oter);
            getline(&line, &len, fp);
            strcpy(acct_ary[i].password, line);
            acct_ary[i].password[strcspn(acct_ary[i].password,"\n")] = '\0';
            memcpy(save_ary[i].password,acct_ary[i].password,strlen(acct_ary[i].password)+1);
            getline(&line, &len, fp);
            acct_ary[i].balance = atof(line);
            float initSavings = 0;
            initSavings = acct_ary[i].balance * 0.2;
            save_ary[i].balance = initSavings;
            getline(&line, &len, fp);
            acct_ary[i].reward_rate = atof(line);
            save_ary[i].reward_rate = 0.02f;
            int c;
            c = pthread_mutex_init(&acct_ary[i].ac_lock, NULL);
            // accounts populated, proceed to create output files
            create_acct_outfiles(i);
            create_save_outfiles(i);
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

void *monitor_transactions()
{
    for(;;)
    {
        if (*processCounter >= processThreshold)
        {
            if (debugText > 0)
            {
                printf("Monitor Triggered - Locking Accounts For Update\n");
                //sleep(1);
            }
            *allowWork = 0;
            for (int i = 0; i < *numAcct; i++) 
            {
                pthread_mutex_lock(&acct_ary[i].ac_lock);
            }
            if (debugText > 0)
            {
                printf("Monitor Locks Acquired - Updating Accounts\n");
                //sleep(1);
            }
            update_balance();
            for (int i = 0; i < *numAcct; i++) 
            {
                pthread_mutex_unlock(&acct_ary[i].ac_lock);
            }
            if (debugText > 0)
            {
                printf("Monitor Locks Released - Signalling Continue\n");
                //sleep(1);
            }
            *processCounter = 0;
            *allowWork = 1;
            pthread_cond_broadcast(&working);
        }
        else if (*monitoring == 0)
        {
            break;
        }
    }
    pthread_exit(NULL);
}

// notice free of pointer here, may be useful elsewhere
void *process_worker_queue(void *i)
{
    int id = *((int *)i);
    int job = 0;
    if (debugText > 0)
    {
        printf("Worker %d Created, Signalling Barrier\n",id);
    }
    pthread_barrier_wait(&startupBarrier);
    if (debugText > 0)
    {
        printf("Startup Signal Received, Beginning Process\n");
    }
    command_line token_buffer;
    int processing = 1;
    while (processing == 1)
    {
        //if (strlen(process_queue[id][job]) == 0)
        if (strcmp(process_queue[id][job],"") == 0)
        {
            processing = 0;
            break;
        } else
        {
            token_buffer = str_filler(process_queue[id][job], " ");
            process_transaction(&token_buffer);
            job++;
        } 
    }
    if (debugText > 0)
    {
        printf("Worker %d Done, Exiting\n",id);
    }
    free_command_line (&token_buffer);
	memset (&token_buffer, 0, 0);
    free(i);
    pthread_exit(NULL);
}

// for each process sentence, execute and update counter
void process_transaction(command_line *token_buffr)
{
    // likely modify how these are passed
    //command_line *token_buffr = (command_line *)token_buf;
    command_line token_buffer = *token_buffr;
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
                    if (*allowWork == 0)
                    {
                        if (debugText == 1)
                        {
                            printf("Wait Triggered on Worker Thread\n");
                        }
                        pthread_cond_wait(&working, &acct_ary[i].ac_lock);
                    }
                    acct_ary[i].balance += amount;
                    acct_ary[i].transaction_tracter += amount;
                    pthread_mutex_unlock(&acct_ary[i].ac_lock);
                    *processCounter = *processCounter + 1;
                } else if (strcmp("W",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1)
					{
						printf(" - Withdrawal Accepted\n");
					}
                    double amount = atof(token_buffer.command_list[3]);
                    pthread_mutex_lock(&acct_ary[i].ac_lock);
                    if (*allowWork == 0)
                    {
                        if (debugText == 1)
                        {
                            printf("Wait Triggered on Worker Thread\n");
                        }
                        pthread_cond_wait(&working, &acct_ary[i].ac_lock);
                    }
                    acct_ary[i].balance -= amount;
                    acct_ary[i].transaction_tracter += amount;
                    pthread_mutex_unlock(&acct_ary[i].ac_lock);
                    *processCounter = *processCounter + 1;
                } else if (strcmp("T",token_buffer.command_list[0]) == 0) 
                {
					if (debugText == 1)
					{
						printf(" - Transfer Initiated\n");
					}
                    double amount = atof(token_buffer.command_list[4]);
                    for (int j = 0; j < *numAcct; j++) 
                    {
                        if (strcmp(acct_ary[j].account_number, token_buffer.command_list[3]) == 0)
                        {
                            pthread_mutex_lock(&acct_ary[i].ac_lock);
                            if (*allowWork == 0)
                            {
                                if (debugText == 1)
                                {
                                    printf("Wait Triggered on Worker Thread\n");
                                }
                                pthread_cond_wait(&working, &acct_ary[i].ac_lock);
                            }
                            acct_ary[i].balance -= amount;
                            acct_ary[i].transaction_tracter += amount;
                            pthread_mutex_unlock(&acct_ary[i].ac_lock);
                            *processCounter = *processCounter + 1;
                            pthread_mutex_lock(&acct_ary[j].ac_lock);
                            if (*allowWork == 0)
                            {
                                if (debugText == 1)
                                {
                                    printf("Wait Triggered on Worker Thread\n");
                                }
                                pthread_cond_wait(&working, &acct_ary[j].ac_lock);
                            }
                            acct_ary[j].balance += amount;
                            pthread_mutex_unlock(&acct_ary[j].ac_lock);
                        }
                    }
                } else 
                {
					if (debugText == 1)
					{
						printf(" - Command Not Recognized\n");
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
    if (debugText > 0)
    {
        printf("Updating Balances... \n");
    }
    *updateSavings = 1;
    kill(pid_array[1],SIGCONT);
    sched_yield();
    //kill(pid_array[1],SIGSTOP);
    //printf("PID 0: %d\nPID 1: %d\n",pid_array[0],pid_array[1]);
    *updateCount = *updateCount + 1;
    for (int i = 0; i < *numAcct; i++) 
    {
        double temp = (acct_ary[i].transaction_tracter * acct_ary[i].reward_rate);
        acct_ary[i].balance += temp;
        acct_ary[i].transaction_tracter = 0;
        char *filename;
        filename = strdup(acct_ary[i].out_file);
        FILE * afp = fopen(filename, "a");
        if (afp == NULL) 
        {
            printf("Failed to open account file to update!\n");
            return;
        } else 
        {
            fprintf(afp,"Current Balance:\t");
            fprintf(afp,"%.2f\n",acct_ary[i].balance);
        }
        fclose(afp);
        free(filename);
    }
    return;
}

void update_savings()
{
    if (debugText > 0)
    {
        printf("Updating Savings... \n");
    }
    for (int i = 0; i < *numAcct; i++) 
    {
        double savings = (save_ary[i].balance * save_ary[i].reward_rate);
        save_ary[i].balance += savings;
        char *savefile;
        savefile = strdup(save_ary[i].out_file);
        FILE * sfp = fopen(savefile, "a");
        if (sfp == NULL) 
        {
            printf("Failed to open savings file to update!\n");
            return;
        } else 
        {
            fprintf(sfp,"Current Savings Balance  ");
            fprintf(sfp,"%.2f\n",save_ary[i].balance);
        }
        fclose(sfp);
        free(savefile);
    }
    // signal update to continue
    return;
}

void puddles()
{
    while(1)
    {
        //printf("puddles waiting...\n");
        if(*updateSavings == 1)
        {
            //printf("------ puddles triggered ------\n");
            //kill(pid_array[0],SIGSTOP);
            update_savings();
            *updateSavings = 0;
            kill(pid_array[0],SIGCONT);
        } else if (*killSavings == 1)
        {
            return;
        } else
        {
            sched_yield();
        }
    }
    return;
}