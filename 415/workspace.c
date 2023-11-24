/*
Library
○ <pthread.h>

● Functions you need
○ pthread_create()
○ pthread_exit()
○ pthread_join()
○ pthread_mutex_lock()
○ pthread_mutex_unlock()
○ pthread_cond_wait()
○ pthread_cond_broadcast() or pthread_cond_signal()
○ pthread_barrier_wait()
○ pthread_barrier_init()
○ sched_yield() (optional but strongly recommended)
○ mmap()
○ munmap()
○ memcpy()

● Functions you have to write
○ void* process_transaction (void* arg)
    ■ This function will be run by a worker thread to handle the transaction
    requests assigned to them.
    ■ This function will return nothing (optional)
    ■ This function will take in one argument, the argument has to be one of the
    following types, char**, command_line*, struct (customized).
○ void* update_balance (void* arg)
    ■ This function will be run by a dedicated bank thread to update each
    account’s balance base on their reward rate and transaction tracker.
    ■ This function will return the number of times it had to update each account
    ■ This function does not take any argument

//////////////////

Input file structure:
● First line
Line 1: n total # of accounts

● Account block
Line n: index # indicating the start of account information
Line n + 1: #........# account number (char*)
Line n + 2: ****** password (char*)
Line n + 3: ###### initial balance (double)
Line n + 4: #.## reward rate (double)

● Transaction lines (separated by space)
Transfer funds:  “T src_account password dest_account transfer_amount”
Check balance:   “C account_num password”
Deposit:         “D account_num password amount”
Withdraw:        “W account_num password amount”

● Additional information
In these requests, there are some invalid requests (wrong password). Getting to know the total
number of each type of request and invalid requests will help you debug down the road.
*/

// See Project Description For Expected Output and Additional Info

/* Part One - Single Threads Solution

Create array of account structs. 
Parse number of accounts (given in first line) * number of fields per account (5). 
This many lines, (plus initial line), lets us store all accounts (initial setup), then we can begin transactions
(ex '10' accts, 5 fields (constant) = 5 * 10 + 1 lines parsed prior to transactions - txn's begin at line 52)
Input fills: index of array, account_number, password, balance (starting), reward_rate
Solvable - transaction_tracter, out_file

transaction_tracter (likely) keeps track of the number of transactions an account has made.
This number will effect the reward rate and final amount. 
Alternatively, this could be a method of calculating a pre-total prior to interacting with balance...

Balance should only update after reward rate and tracter calcuations - the update is called deliberately at threshold or end of file. 
Balance update adds the current balance to tracter * reward rate

Whenever a deposit, withdrawal, or transfer is made, the amount is added to the tracker (tracter). 
We must keep a counter of transactions (EXCLUDING Check Balance) across all threads.

Deposit - add to accounts tracker
Withdrawal - subtract
Transfer - subtract from source tracker, add to dest tracker
Check - simply print unupdated balance. 

*/

/* Part Two - Multi-thread w/ Critical Section

Idenfity critical sections - believe to be any that modify the account class (aka process transactions)
// plus the setup, but this should be handled prior to threads

Do any other sections modify globals?

Use pthread_mutex_t (w/in account) to lock sections

"Evenly slice the number of transactions for each of the of the worker threads to handle based on number of workers"
Having number of lines in the file, subtract lines required for account creation.

Dividing this result by number of threads gives each threads starting location for processing. ? will effect intermediate results, as its non sequential
Attempt to spawn ten consecutive threads? Must null check!

Use pthread_create to start all worker threads, (sliced above)

Use pthread create to start the "bank thread". This only updtaes when all workers complete. 

*/

/* Part Three - Coordinating the Threads (to work together)

Update reward regularly (every 5000 transactions, not including check balance)

use pthread_barrier_wait & pthread_cond_wait to facilitate communication between worker and bank threads

Have all threads wait for signal after creation. 

When legal/legit (non check balance) requests reaches 5000, all threads must pause and notify bank to wake up

When updating balances, each account will have individual output files written to. 

When writing completes, the bank thread will tell each worker to continue, placing itself back in a ready state.

*/

/* Part Four - Transferring to Another Bank (savings acct)

A second process, similar to the first (overarching program aka main), will populate the same account data. 
Initial balance will equal 20% of the original initial (ie 1 duck checking = 0.2 puddles savings)

Everyone has a flat reward rate of 2%

We want duck bank processes to write to shared memory using mmap()

Every time banker thread from part 3 applies interest, (update_balance),
puddles should apply the .02 rate to the savings balance as well

Savings accounts should go to a savings directory (instead of output) but otherwise be the same format

*/

/* Additional Remarks:

Race conditions are going to play a huge role while implementing part3. An important
question you should ask yourself is how do you make sure one thread will reach a certain part of
the code before another?

Deadlocks could make your program stuck, and it is extremely difficult to figure out
exactly what happened, and how to resolve it. Think about what variables you could keep track
of to signal a deadlock.

Output should include any useful information about the state of the program. 
Last line should read 'total updates: x'

Output should include thread ID's, detailing when they start, pause to signal bank, 
when bank receives signal, when bank is waiting, and when threads complete. See example. 

*/