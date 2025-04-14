우선 xv6에서 system call이 어떻게 내부적으로 동작하는지 알아야한다.



## 파일의 역할

sysproc.c : systemcall 함수가 구현되어있는곳

syscall.c : systemcall table이 있는곳, 각각의 system call 함수를 숫자로 나타내고있다.

syscall.h : system call 번호 정의

user.h : user level에서 호출하는 함수를 정의하고 있는곳

usys.S : user level에서 호출하는 함수를 감싸고 있는곳



usys.S를 통해 user.h에 정의되어있는 함수가 sysproc.c 내부에 있는 sys_명령어()의 형태로 커널내부에서 필요한 정보를 가져올 수 있는것이다.



사용자 프로그램에서 시스템 콜 함수를 호출하면 해당 인자들이 사용자 스택에 저장되고 system call 진입시 이 스택에서 몇번째 인자인지를 기준으로 꺼낸다

argptr, argint의 경우 이 인수가 올바른 사용자 영역에 있는지 검사하고 그 메모리 주소를 커널이 사용할 수 있게 넘겨주는 역할을 한다.

---

1. proc.h에 procinfo 구조체를 정의한다.
```
struct procinfo{
int pid; //process id
int ppid; //parent process id
int state; //process state
uint sz; //memory size
}
```

프로세스 상태값은 enum이지만 상태를 문자열로 출력하기 위해 process state값을 int로 하였다.

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
proc.h에 정의 되어있는 procstate이다.


---
2. sys_getprocinfo() 구현
```
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
```
userspace에 myproc()이 반환하는 proc구조체 포인터변수를 그대로 반환하면 call by reference를 하게되므로 userspace에서 kernel내부 변수에 접근가능하게 된다. 그렇게 되면 안되므로 myprocinfo에 현재 프로세스의 정보값을 다시 새로운 구조체 변수에 담아준다.  또한, 정보를 담는 myprocinfo포인터변수의 위치가 커널공간에 있으면 안되므로 argptr함수를 이용하여 예외처리를 해준다.





```
int argptr(int n, char **pp, int size)
```
n : 현재 systemcall함수(sys_procinfo())를 호출한 함수의 n번째 parameter를 의미한다.

*호출함수 : ```int getprocinfo(struct procinfo*); ```

pp : user space 내에 있는지 검사할 포인터변수 대상

size : pp가 가리키는 구조체의 크기





user space에 넘겨줄 myprocinfo 포인터변수가 가리키는 변수가 사용자공간 내에 있는지 확인

예외상황 : getprocinfo의 0번째 인자가 user space 내에 없는경우


---
3. system call 테이블에 sys_getprocinfo() 추가하고 syscall.h에 시스템콜 번호 선언하기

- syscall.c

  ```extern int sys_getprocinfo(void); //과제 1 - system call table```

  ```[SYS_getprocinfo] sys_getprocinfo, //과제 1 - system call table```



- syscall.h

  ```#define SYS_getprocinfo 22 //과제 1```





---
4. user space에서 호출할 getprocinfo(struct procinfo*) user.h와 usys.S에 정의하기

- usys.S

  ```SYSCALL(getprocinfo) //과제1```

- user.h

  ```int getprocinfo(struct procinfo*); //과제1 - userspace wrapper```




---
5. procinfotest.c 작성

예외 : 현재 프로세스의 부모프로세스가 없는경우(e.g. init) 

예외 상황의 경우 Parent Process Id 가 -1로 반환된다. 다음과 같이 예외처리를 해주었다.

```
if(curprocinfo->ppid == -1)
printf(1, "Process %d has no parent process\n", curprocinfo->pid);
else
printf(1, "Parent Process ID: %d\n", curprocinfo->ppid);
```


user가 procinfotest명령어를 입력하면 sh.c파일에서 프로세스를 새로 형성하기 때문에 procinfotest.c의 메인함수에서 반드시 exit()을 해주어야한다.


---
procinfotest는 명령어를 입력할때마다 새로운 프로세스가 형성되기 때문에 결과는 다음과 같이 나온다.


![결과 이미지](https://github.com/hangeunhyeong/xv6/blob/main/TalkMedia_i_5dcb12ba3e0d.png)

---
##정리

shell에서의 사용자의 입력으로 fork()가 실행되면서 자식프로세스 생성됨 ->

getprocinfo()호출 (user level) ->

user.h, usys.S, syscall.h에 정의된 시스템콜 번호가 mapping됨 ->

sys_getprocinfo() 호출 후 수행(kernel space) -> 종료
