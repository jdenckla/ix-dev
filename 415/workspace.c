// For drafting/planning

// Instructions: 
/*

Part 1: Lab 1

Read from a file, each line is a command and its args

Take an input file, get number of lines. Does the token function give us this?

For each line in the array, fork()

Attempt to exec this line. 

-> pull from prev proj

For each command, MCP must launch a separate process to run the command:
fork() the proc
an exec() variant to run it

-> pull from lab 1

Once all proc's are running, MCP should wait for each to terminate
using a wait() variant

-> pull from lab 1

After all proc's termainate, MCP must use exit()

-> pos lab 2

*/

///////////////////////////////////////////////////////

/*

Part 2: Lab 2

Between fork() and exec(), tell each process to stop and wait for SIGUSR1
-> implement sigwait() as with lab5, which occurs immediately after fork()
->> SIGUSR1 is the first signal to wait for

Once each process is created, (and therefore waiting), send each child SIGUSR1 simultaneously
-> This will allow each to reach exec() call, see lab5...

Once each process begins executing, (after exec()), send each process SIGSTOP
-> should be similar to the above...

After all processes ahve been touched by SIGSTOP, send each SIGCONT
-> ||

After SIGCONT, wait for each process to finish, (same as part 1), then free associated memory

*/

/*

Part 3: No more labs to carry us..

Use alarm(), (which delivers the SIGALARM signal), to effectively time each process
-> Each process will only be given a time slice to execute in

In using this alarm(), we will do the following:
-> suspend current workload process using SIGSTOP
->> Determine next workload process and send it SIGCONT
->>> Reset the alarm and continue whatever else...

Ensure that the above transitions output some kind of debug.

Notes:

Each workload process is a child of the MCP (aka part3). How do we know each has terminated?
-> possibly modify the value inside of pid_array?
-X Cannot use the return value of waitpid.

Ensure output shows the steps above, (the start/stop, the alarm, possibly a process ending)

// pos look into 'quantum = getenv()' ?

Potentially consider using a global array (or two) to store process information, to include whether each process is running. 
Fields to include: PID, IsRunning (bool), Parent -> or just wait til Part 4

Potentially make a FIFO queue, (with push/pop), to rotate between active processes
-> how to determine whether a process is active
->> how to pop when a process finishes

! Look at arrival time of each thread

Pos use sched_yield after alarm? Don't know that we want to touch the kernel though

One (unlikely) solution:
-> Create a linked list, (building struct into code and main). This would be global, shared across processes. 
!Either! Append the queue (at tail end) when the process is forked, OR append the queue when the process awaits execution

->> Starting at the head, pop the process off the queue, exec for alloted time***, 
then IF process has not finished, (see above), push back to the tail of the queue

->>> Processes can be started/stopped with STOP/CONT signals. Timer can be ALARM. INT can interrupt the current process and move to next? Pos just have it pause until next press.
SIGUSR1 may be for the queueing function, (ie fork all before processing any)

?> Should we look at arrival time when building the queue? If we had a tree we wouldn't want parents running before children.
-> perhaps arrival time can help sort the queue initially? (ie later arrival, put at front of list to ensure hierarchy rather than FIFO)

??> Do we concern ourselves with the initial parent process? (In this case, part3)
*/

/*

Part 4:

"Simple" test/debug output. Decide what to lookup, analyze, and present.

Each process exists in the /proc directory, (numbered). Each has:
-> /proc/<PID>/cmdline ~ gives cmd line arguement(s) used to start the process
-> /proc/<PID>/status ~ including mem usage and proc statistics, see 'Name' and 'State'
-> /proc/<PID>/fd ~ shows links to files opened by process

We'll want similar functionality to 'top' where we print information into a table and update it at every cycle.

*/
