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

*/
