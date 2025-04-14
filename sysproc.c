#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
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
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//과제1 - kernel implementation
int
sys_getprocinfo(void){
    struct procinfo *myprocinfo;
    struct proc *curproc = myproc(); 
    
    // n번째 인자의 메모리주소가 사용자 공간 내에 있는지 확인하는 함수
    if(argptr(0, (char **)&myprocinfo, sizeof(struct procinfo)) < 0){
      return -1;
    }
    myprocinfo->pid = curproc->pid;
    //현재 프로세스의 부모가 없을경우(e.g. init)
    myprocinfo->ppid = (curproc->parent) ? curproc->parent->pid : -1;
    myprocinfo->state = curproc->state;
    myprocinfo->sz = curproc->sz;

    return 0;
}
