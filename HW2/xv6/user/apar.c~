/*#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

#define NPROC 64

struct pstat {
    int inuse[NPROC];  // whether this slot of the process process table is in use (1 or 0)
    int pid[NPROC];    // the PID of each process
    int hticks[NPROC]; // the number of ticks each process has accumulated at priority 2
    int lticks[NPROC]; // the number of ticks each process has accumulated at priority 1
};

int main(int argc, char* argv[]){
	int i = 0;
	printf(1, "In main()");
	int j = setpri(atoi(argv[1]));
	printf(1,"\nSetpri returned %d", j);
	//setpri(2);
	struct pstat *p = (struct pstat*)malloc(sizeof(struct pstat));
	if(p == NULL){
		printf(1, "\n Error allocating memory for pstat!");
	}else{
		printf(1, "\nMemory allocation successful!");
	}
	p->pid[0] = 111;
	printf(1, "\n After assigning, p->pid[0] = %d", p->pid[0]);
	int x = getpinfo(p);
	printf(1, "Called getpinfo(); return value is %d", x);
	
	if(x == -1){
		printf(1, "\nFailure!");
	}else{
		printf(1, "Return value : %d", x);
		for(i = 0; i < NPROC; i++){
			printf(1,"\n*****");
			printf(1,"\n Process pid : %d", p->pid[i]);
			printf(1,"\n Process lticks : %d", p->lticks[i]);
			printf(1,"\n Process hticks : %d", p->hticks[i]);
			printf(1,"\n*****");
		}
	}
	exit();
}*/

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();}

void spin()
{
	int i = 0, j = 0, k = 0;
	for(i = 0; i < 50; ++i)
	{
		for(j = 0; j < 10000000; ++j)
		{
			k = j % 10;
		}
	}
}

void print(struct pstat *st)
{
   int i;
   for(i = 0; i < NPROC; i++) {
      if (st->inuse[i]) {
         printf(1, "pid: %d hticks: %d lticks: %d\n", st->pid[i], st->hticks[i], st->lticks[i]);
      }
   }

}

void compare(struct pstat *before, struct pstat *after)
{
	int i, processesrun = 0;
	for(i = 0; i < NPROC; i++)
	{
		check(before->inuse[i] == after->inuse[i], "number of processes inuse viewed at different times should be same");
		check(before->pid[i] == after->pid[i], "the pid of processes at a particular index taken viewed at different times should be same");
		check(before->lticks[i] == after->lticks[i], "no low priority process should have been run when there is a runnable high priority process");
		if(before->hticks[i] < after->hticks[i])
			processesrun++;
	}
	check(processesrun == 1, "Expecetd that only the high prirority process is run when there is one");
}

int
main(int argc, char *argv[])
{
   if(fork() == 0)
   {
   	struct pstat st_before, st_after;
	check(setpri(2) == 0, "setpri");
	check(getpinfo(&st_before) == 0, "getpinfo");
	printf(1, "\n ****PInfo before**** \n");
	print(&st_before);
	spin();
	spin();
	check(getpinfo(&st_after) == 0, "getpinfo");
	printf(1, "\n ****PInfo after**** \n");
	print(&st_after);
	compare(&st_before, &st_after);
	printf(1, "Should print 1"); 
	exit();
   }
   spin();
   printf(1, " then 2");
   exit();
}

