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
int safebounds (struct proc *p, uint addr){
  // partC- change to 1 // 4*
  if (p->pid!=1 && (addr < PGSIZE || addr >=USERTOP || (addr >= p->sz && addr < p->stack_bm))) {
    return 1;
  } else if(addr == 0) {
    return 1;
  }else {
    for (int i = 0; i <= 2; i++){
      if (p->sharedpg[i]==-1 && addr >= (i+1)*PGSIZE && addr < 4*PGSIZE){
        return 1;
      }
    }
  }
  return 0;
}
int
fetchint(struct proc *p, uint addr, int *ip)
{
  // NEW
  if(safebounds(p,addr) || addr+4 > USERTOP)
  //New End
  { return -1;}
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(struct proc *p, uint addr, char **pp)
{
  char *s, *ep;
  //NEW
  if(safebounds(p,addr) == 1)
  // New END
  {return -1;}
  *pp = (char*)addr;
  int j;
  for (j = 0; j <= 2; j++){
    if (p->sharedpg[j]==-1){
      break;
    }
  }
  if (addr < (uint)(j+2)*PGSIZE && j < 2){ // can access the code page
    ep = (char*) ((uint)(j+2)*PGSIZE);
  } else if (addr < p->sz){
    ep = (char*)p->sz;
  } else if (addr < USERTOP){
    ep = (char*)USERTOP;
  }
    for(s = *pp; s < ep; s++)
      if(*s == 0)
        return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint(proc, proc->tf->esp + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;

  if(argint(n, &i) < 0){
    return -1;
  }
  // NOTE: i + size is grater than the mem they alloc, and this is a user process
  // partC change to 1 // 4*
  if ((uint)i < PGSIZE) return -1;
  // New - partC
  if ((uint)i < 4*PGSIZE){
    for (int j = 0; j <= 2; j++){
      if (proc->sharedpg[j]==-1 && ((uint)i >= (j+1)*PGSIZE || (uint)i+size > (j+1)*PGSIZE)){
        return -1;
      }
    }
  }
  // New end
  if(((uint)i >= proc->sz || (uint)i+size > proc->sz) && ((uint)i < proc->stack_bm || (uint)i+size > USERTOP)){
    return -1;
  }
  for (int j = 0; j <= 2; j++){
    if (proc->sharedpg[j]==-1 && (uint)i >= (j+1)*PGSIZE && (uint)i < 4*PGSIZE){
    }
  }
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
  int addr;
  if(argint(n, &addr) < 0)
  {return -1;}
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
[SYS_shmget]  sys_shmget,
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
