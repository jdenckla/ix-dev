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


*/

/* Part Two - Multi-thread w/ Critical Section

*/

/* Part Three - Coordinating the Threads (to work together)

*/

/* Part Four - Transferring to Another Bank (savings acct)

*/

/* Additional Remarks:

Race conditions are going to play a huge role while implementing part3. An important
question you should ask yourself is how do you make sure one thread will reach a certain part of
the code before another?

Deadlocks could make your program stuck, and it is extremely difficult to figure out
exactly what happened, and how to resolve it. Think about what variables you could keep track
of to signal a deadlock.

*/