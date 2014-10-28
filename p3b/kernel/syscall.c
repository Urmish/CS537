#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "sysfunc.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from process p.
int
fetchint(struct proc *p, uint addr, int *ip)
{
  //cprintf("\n Urmish - fetchint called by : %s \n",p->name);
  //cprintf("\n Urmish - fetchint sz: %d addr: %d \n",p->sz,addr);
  //if(addr < p->stack_low || addr > USERTOP || addr+4>USERTOP)
  if(!((addr > PGSIZE && addr < p->sz && addr + 4 < p->sz) || (addr > p->stack_low && addr < USERTOP && addr+4<USERTOP)))
    return -1;
  *ip = *(int*)(addr);
  //cprintf("\n Urmish - ip is %d \n",*ip);
  return 0;
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(struct proc *p, uint addr, char **pp)
{
  char *s, *ep;

  if(addr >= USERTOP)
    return -1;
  *pp = (char*)addr;
  //Added by Urmish
  if (addr < p->sz)
  {
      ep = (char*)p->sz;
  }
  else if (addr < USERTOP && addr > p->stack_low)
  {
      ep = (char*)USERTOP;
  }
  //cprintf("\n Urmish - fetchstr called by : %s \n",p->name);
  //cprintf("\n Urmish - fetchstr sz: %d addr: %d\n",p->sz,addr);
  for(s = *pp; s < ep; s++)
    if(*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  //cprintf("\n Urmish - argint called by : %s \n",proc->name);
  //cprintf("\n Urmish - argint ip: %p n: %d \n",ip,n);
  return fetchint(proc, proc->tf->esp + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  
  //cprintf("\n Urmish - argptr called by : %s \n",proc->name);
  //cprintf("\n Urmish - argptr n: %d, size:%d \n",n,size);
  if(argint(n, &i) < 0)
    return -1;
  //cprintf("\n Urmish - argptr i \n",i);
  if((uint)i >= USERTOP || (uint)i+size > USERTOP) //Urmish Change here`
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  //cprintf("\n Urmish - argstr called by : %s",proc->name);
  //cprintf("\n Urmish - argstr n: %d",n);
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(proc, addr, pp);
}

// syscall function declarations moved to sysfunc.h so compiler
// can catch definitions that don't match

// array of function pointers to handlers for all the syscalls
static int (*syscalls[])(void) = {
[SYS_chdir]   sys_chdir,
[SYS_close]   sys_close,
[SYS_dup]     sys_dup,
[SYS_exec]    sys_exec,
[SYS_exit]    sys_exit,
[SYS_fork]    sys_fork,
[SYS_fstat]   sys_fstat,
[SYS_getpid]  sys_getpid,
[SYS_kill]    sys_kill,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_mknod]   sys_mknod,
[SYS_open]    sys_open,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_unlink]  sys_unlink,
[SYS_wait]    sys_wait,
[SYS_write]   sys_write,
[SYS_uptime]  sys_uptime,
};

// Called on a syscall trap. Checks that the syscall number (passed via eax)
// is valid and then calls the appropriate handler for the syscall.
void
syscall(void)
{
  int num;
  
  num = proc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num] != NULL) {
    proc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            proc->pid, proc->name, num);
    proc->tf->eax = -1;
  }
}
