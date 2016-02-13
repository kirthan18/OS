#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_setpri(void)
{
  int pri;
  //cprintf("In sys_setpri()");
  if(argint(0, &pri) < 0){
  	return -1;
  }
  return setpri(pri);
}

int
sys_getpinfo(void)
{
  struct pstat *p;
  int y;
  int i = 0;
  //cprintf("\n Inside sys_getpinfo in sysproc.c");
  if(argptr(0, (void*)&p, sizeof(*p)) < 0){
	//cprintf("\n In sysproc = failure!");
	return -1;
  }
  //cprintf("\nRunning mytest : pid[0] = %d", p->pid[0]);
  //cprintf("\n Calling system call..");
  y = getpinfo(p);
  for(i = 0; i < NPROC; i++){
	//cprintf("\n Checking index %d", i);
	if(p->pid[i] != 0){
		//cprintf("\nProcess id in sys_getpinfo : %d", p->pid[i]);
	}
  }
  //cprintf("\n Return value from getpinfo() is %d", y);
  return y;
}
