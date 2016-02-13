#define NPROC 64

struct pstat {
	            int inuse[NPROC];  // whether this slot of the process process table is in use (1 or 0)
		    int pid[NPROC];    // the PID of each process
		    int hticks[NPROC]; // the number of ticks each process has accumulated at priority 2	
		    int lticks[NPROC]; // the number of ticks each process has accumulated at priority 1
};
