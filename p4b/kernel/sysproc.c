#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_clone(void)
{
  void* stack;

  //cprintf("\n*****Clone called by proc %d %s *****\n",proc->pid, proc->name);  
 
  if (argptr(0,(char **)&stack,sizeof(int))<0)
  {
    cprintf("\n***** ERROR!!! sysproc.c : sys_clone ***** \n");
    return -1;
  }
  //cprintf("\n*sys_clone has been called*\n");
  return clone(stack);
}

int
sys_lock(void)
{
  int* lock_int;

  //cprintf("\n*****LOCK called by proc %d %s *****\n",proc->pid, proc->name);  
 
  if (argptr(0,(char **)&lock_int,sizeof(int))<0)
  {
    cprintf("\n***** ERROR!!! sysproc.c : sys_lock ***** \n");
    return -1;
  }
  //cprintf("\n*sys_lock has been called*\n");
  return get_lock(lock_int);
}

int
sys_unlock(void)
{
  int* unlock_int;

  //cprintf("\n*****UNLOCK called by proc %d %s *****\n",proc->pid, proc->name);  
 
  if (argptr(0,(char **)&unlock_int,sizeof(int))<0)
  {
    cprintf("\n***** ERROR!!! sysproc.c : sys_lock ***** \n");
    return -1;
  }
  //cprintf("\n*sys_unlock has been called unlock_int is %p*\n", unlock_int);
  *unlock_int = 0;
  wakeup((void *)unlock_int);
  return 0;
}

int
sys_join(void)
{
  //cprintf("\n*sys_join has been called*\n");
  return join();
}

int
sys_fork(void)
{
  //cprintf("\n*****Fork called by proc %d %s *****\n",proc->pid, proc->name);  
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
