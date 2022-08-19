# Lab2 System Calls

在本实验中，需要向xv6中增加一些新的系统调用

**Lab2前置要求**

- 阅读xv6 book的第2章、第4章4.3以及4.4
- 系统调用的用户空间代码在`user/user.h`和`user/usys.pl`中
- 内核空间代码是`kernel/syscall.h`， `kernel/syscall.c`
- 与进程相关的代码是`kernel/proc.h`和`kernel/proc.c`

## System call tracing

- 将trace的函数原型加入`user.h`

  ```C
  // user.h
  int trace(int);
  ```

- 将存根加入usys.pl

  ```C
  // usys.pl
  entry("trace");
  ```

- 根据第二条hint，在`syscall.h`中加入系统调用号(macro)

  ```C
  #define SYS_trace  22
  ```

- 在`kernel/sysproc.c`中添加一个`sys_trace`函数

  ```C
  uint64
  sys_trace(void)
  {
    int mask;
    if(argint(0, &mask) < 0) return -1;
    myproc()->mask = mask;
  
    return 0;
  }
  ```

- 在`proc`结构体中加入mask对应上述函数的mask变量

  ```C
  struct proc{
    // ...
    int mask;
  };
  ```

- 对syscall中的`syscall`函数进行修改，使其输出trace跟踪的结果

  ```C
  void
  syscall(void)
  {
    const char *syscall_names[] = {"fork", "exit", "wait", "pipe", "read",
    "kill", "exec", "fstat", "chdir", "dup", "getpid", "sbrk", "sleep",
    "uptime", "open", "write", "mknod", "unlink", "link", "mkdir", "close", "trace", "sysinfo"};
  
    int num;
    struct proc *p = myproc();
  
    num = p->trapframe->a7;
    if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
      p->trapframe->a0 = syscalls[num]();
      if ((p->mask) & (1 << num)) printf("%d: syscall %s -> %d\n", p->pid, syscall_names[num-1], p->trapframe->a0);
    } else {
      printf("%d %s: unknown sys call %d\n",
              p->pid, p->name, num);
      p->trapframe->a0 = -1;
    }
  }
  ```

- 同时需要在`syscall.c`中增加对于函数的声明

  ```C
  // kernel/syscall.c
  extern uint64 sys_trace(void);
  
  static uint64 (*syscalls[])(void) = {
  // ...
  [SYS_trace]   sys_trace,
  
  };
  ```

- 在`fork`函数中，将父进程的mask值复制给子进程

  ```C
  // kernel/proc.c
  int
  fork(void)
  {
    ...
    np->mask = p->mask;
    ...
  }
  ```

## Sysinfo

在最开始的user.h以及usys.pl文件中的操作与前面的trace非常类似，在此不做赘述

- 由于sysinfo需要统计当前系统的相关信息 ===> 剩余可用空间/已用线程数 ===> 需要对应的函数

  ```C
  // nproc ===> kernel/proc.c
  uint64 
  nproc(void)
  {
    uint64 num = 0;
    for(int i=0; i<NPROC; i++){
      if(proc[i].state != UNUSED) num++;
    }
    return num;
  }
  // freemem ===> kernel/kallo.c
  uint64
  freemem(void)
  {
    uint64 size = 0;
    struct run *r = kmem.freelist;
    int cnt = 0;
    while(r){
      cnt++;
      r = r->next;
    }
    size = cnt*PGSIZE;
  
    return size;
  }
  ```

- 在`kernel/sysproc.c`中添加一个`sys_trace`函数

  ```C
  uint64
  sys_sysinfo(void)
  {
    uint64 addr;
    if(argaddr(0, &addr) < 0) return -1;
    struct proc *p = myproc();
    struct sysinfo sinfo;
    sinfo.freemem = freemem();
    sinfo.nproc = nproc();
  
    if(copyout(p->pagetable, addr, (char *)&sinfo, sizeof(sinfo)) < 0) return -1;
    return 0;
  }
  ```

- 和sys_trace相同，需要在`syscall.c`中增加函数的声明

  ```C
  // kernel/syscall.c
  extern uint64 sys_sysinfo(void);
  
  static uint64 (*syscalls[])(void) = {
  // ...
  [SYS_sysinfo]   sys_sysinfo,
  
  };
  ```

## 心得

- hint很重要，其中会给出完成整个实验的需要完成的事(几乎每一步都有涉及到)

- 对于各种.c文件中新增的函数，函数声明应当放在`kernel/defs.h`中；一度因为这个问题迷惑很久

  

