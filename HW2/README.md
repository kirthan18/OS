Project 2a - Shell Implementation - Whoosh
Build No: v1.0
Release Date: 02-14-2016
Author: Kirthanaa Raghuraman <kirthanaa@cs.wisc.edu>

Introduction: 
	This program creates a new shell called "whoosh" and implements some very basic built in commands and other commands using execv.

Usage: 
	./whoosh  

Known issues: 
	None

Bug reporting: 
	File bugs at kirthanaa@cs.wisc.edu 


Project 2b - xv6 Scheduling
Build No: v1.0 
Release Date: 02-14-2016
Author: Kirthanaa Raghuraman <kirthanaa@cs.wisc.edu>

Introduction:
The project aims at implementing a modified scheduling algorithm as follows:
* A process can have either priority 1 (low) or priority 2 (high).
* At any time, only processes with priority 2 and in the runnable state should be executed.
* Only if there is no process with priority 2 in runnable state, should a process with priority 1 be executed.

The following system calls have been added to xv6 kernel: 
int setpri(int) - Sets the priority of the calling process to the input specified.
getpinfo(struct pstat*) - Returns information about the processes' pids, hticks, lticks, state by populating the pstat structure.

Both the system calls return 0 on successful execution and 1 on failure.

Usage: 
	Set priority of calling process  =  setpri(priority)
	Get information about the processes  =  getpinfo(pstat *p)

Known issues: 
	None

Bug reporting: 
	File bugs at kirthanaa@cs.wisc.edu 

