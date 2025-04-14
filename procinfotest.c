#include "types.h"
#include "param.h"
#include "mmu.h"
#include "stat.h"
#include "user.h"
#include "proc.h"
char* 
getstate(int n){
    switch(n){
        case 0: return "UNUSED";    break;
        case 1: return "EMBRYO";    break;
        case 2: return "SLEEPING";    break;
        case 3: return "RUNNABLE";    break;
        case 4: return "RUNNING";    break;
        case 5: return "ZOMBIE";    break;
        default: break;
    }
    printf(1,"%d",n);
    return "NONE";
}
int 
main(void){
    struct procinfo* curprocinfo;
    getprocinfo(curprocinfo);
    
    printf(1, "Process ID : %d\n", curprocinfo->pid);

    //부모 프로세스가 없는경우 ex) init()
    if(curprocinfo->ppid == -1)
        printf(1, "Process %d has no parent process\n", curprocinfo->pid);
    else
        printf(1, "Parent Process ID: %d\n", curprocinfo->ppid);

    printf(1, "Process State: %s\n", getstate(curprocinfo->state));
    printf(1, "Memory Size: %d\n", curprocinfo->sz);

    exit();
}
