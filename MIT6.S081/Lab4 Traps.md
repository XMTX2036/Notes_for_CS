# Lab4 Traps

本实验探究如何使用traps实现系统调用

## Backtrace

backtrace可用于堆栈上发生错误的点上方的函数调用列表；编译器在每个栈帧中放入一个`frame pointer`，`backtrace`通过这些指针向上遍历堆栈打印每个保存的返回地址

- 将`backtrace`的原型加入`kernel/defs.h`中，同时在`sys_sleep`中调用`backtrace`

  ```C
  // kernel/defs.h
  void backtrace(void);
  
  // kernel/sysproc.c
  uint64
  sys_sleep(void)
  {
    backtrace();
    
    // ...
  }
  ```

- `kernel/riscv.h`中声明如下函数用于读取当前帧指针

  ```C
  // inline function with asm volatile to get frame pointer
  static inline uint64
  r_fp()
  {
    uint64 x;
    asm volatile("mv %0, s0" : "=r" (x) );
    return x;
  }
  ```

- 由下列堆栈布局可知：返回地址位于距堆栈帧的帧指针的固定偏移量 (-8) 处，而保存的帧指针位于距帧指针的固定偏移量 (-16) 处

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-28%2013.53.36.png" alt="截屏2022-08-28 13.53.36" style="zoom:50%;" />

  ```C
  void 
  backtrace(void)
  {
    printf("backtrace: \n");
    uint64 ra, fp = r_fp();
    while(fp < PGROUNDUP(fp) && fp > PGROUNDDOWN(fp)){
      ra = *(uint64 *)(fp-8);
      printf("%p\n", ra);
      fp = *(uint64 *)(fp-16);
    }
  }
  ```

**模块测试结果如下**：

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-28%2023.13.15.png" alt="截屏2022-08-28 23.13.15" style="zoom:50%;" />

## Alarm

向 xv6 添加一个功能，该功能会在进程使用 CPU 时间时定期提醒它。这对于想要限制它们占用多少 CPU 时间的计算绑定进程，或者对于想要计算但又想要采取一些定期操作的进程可能很有用。更一般地说，您将实现一种**原始形式的用户级中断/故障处理程序**

- 在Makefile增加`alarmtest`，使其可以成功编译

  ```C
  UPROGS=\
  	$U/_alarmtest\
  	$U/_cat\
  	$U/_echo\
  	$U/_forktest\
  	$U/_grep\
  	$U/_init\
  	$U/_kill\
  	$U/_ln\
  	$U/_ls\
  	$U/_mkdir\
  	$U/_rm\
  	$U/_sh\
  	$U/_stressfs\
  	$U/_usertests\
  	$U/_grind\
  	$U/_wc\
  	$U/_zombie\
  ```

- 在`user/user.pl`、`kernel/syscall.h`、`kernel/syscall.c`等添加`sys_sigalarm`和`sys_sigreturn`这两个syscall的声明

  ```perl
  // user/user.pl
  entry("sigalarm");
  entry("sigreturn");
  
  // kernel/syscall.h
  #define SYS_sigalarm 22
  #define SYS_sigreturn 23
  
  // kernel/syscall.c
  extern uint64 sys_sigalarm(void);
  extern uint64 sys_sigreturn(void);
  
  static uint64 (*syscalls[])(void) = {
  // ...
  [SYS_sigalarm] sys_sigalarm,
  [SYS_sigreturn] sys_sigreturn,
  };
  ```

- 在kernel/proc.h的proc结构体中添加需要的成员变量

  ```C
  struct proc{
    // ...
  	uint64 handler;
    int interval;
    int ticks;
    int allow_entrance_handler;
    // saved registers for proc structure
    uint64 saved_epc;
    uint64 saved_ra;
    uint64 saved_sp;
    uint64 saved_gp;
    uint64 saved_tp;
    uint64 saved_t0;
    uint64 saved_t1; 
    uint64 saved_t2;
    uint64 saved_s0;
    uint64 saved_s1;
    uint64 saved_s2;
    uint64 saved_s3;
    uint64 saved_s4;
    uint64 saved_s5;
    uint64 saved_s6;
    uint64 saved_s7;
    uint64 saved_s8;
    uint64 saved_s9;
    uint64 saved_s10;
    uint64 saved_s11;
    uint64 saved_a0;
    uint64 saved_a1;
    uint64 saved_a2;
    uint64 saved_a3;
    uint64 saved_a4;
    uint64 saved_a5;
    uint64 saved_a6;
    uint64 saved_a7;
    uint64 saved_t3;
    uint64 saved_t4;
    uint64 saved_t5;
    uint64 saved_t6;
  };
  ```

- 在`kernel/sysproc.c`中实现`sys_sigalarm`和`sys_sigreturn`

  ```C
  uint64
  sys_sigalarm(void)
  {
    int interval;
    uint64 handler;
    if(argint(0, &interval) < 0 || argaddr(1, &handler) < 0) return -1;
    struct proc *p = myproc();
    p->interval = interval;
    p->handler = handler;
  
    return 0;
  }
  
  uint64
  sys_sigreturn(void)
  {
    // restore all the saved registers
    struct proc *p = myproc();
    p->trapframe->epc = p->saved_epc; 
    p->trapframe->ra = p->saved_ra; 
    p->trapframe->sp = p->saved_sp; 
    p->trapframe->gp = p->saved_gp; 
    p->trapframe->tp = p->saved_tp; 
    p->trapframe->a0 = p->saved_a0; 
    p->trapframe->a1 = p->saved_a1; 
    p->trapframe->a2 = p->saved_a2; 
    p->trapframe->a3 = p->saved_a3; 
    p->trapframe->a4 = p->saved_a4; 
    p->trapframe->a5 = p->saved_a5; 
    p->trapframe->a6 = p->saved_a6; 
    p->trapframe->a7 = p->saved_a7; 
    p->trapframe->t0 = p->saved_t0; 
    p->trapframe->t1 = p->saved_t1; 
    p->trapframe->t2 = p->saved_t2; 
    p->trapframe->t3 = p->saved_t3; 
    p->trapframe->t4 = p->saved_t4; 
    p->trapframe->t5 = p->saved_t5; 
    p->trapframe->t6 = p->saved_t6;
    p->trapframe->s0 = p->saved_s0;
    p->trapframe->s1 = p->saved_s1;
    p->trapframe->s2 = p->saved_s2;
    p->trapframe->s3 = p->saved_s3;
    p->trapframe->s4 = p->saved_s4;
    p->trapframe->s5 = p->saved_s5;
    p->trapframe->s6 = p->saved_s6;
    p->trapframe->s7 = p->saved_s7;
    p->trapframe->s8 = p->saved_s8;
    p->trapframe->s9 = p->saved_s9;
    p->trapframe->s10 = p->saved_s10;
    p->trapframe->s11 = p->saved_s11;
  
    myproc()->allow_entrance_handler = 1;
    return 0;
  }
  ```

- 每个滴答声，硬件时钟都会强制中断，该中断在``kernel/trap.c``的``usertrap()``中处理

  此时，需要判断`which_dev==2`才是定时器中断，当`p->ticks`到达预设值`p->interval`时，将调用`p->handler`

  ```C
  // kernel/trap.c
  	// give up the CPU if this is a timer interrupt.
    if(which_dev == 2){
      p->ticks += 1;
      if (p->ticks == p->interval && p->allow_entrance_handler) {
        p->allow_entrance_handler = 0;
  
        p->saved_epc = p->trapframe->epc;
        p->saved_ra = p->trapframe->ra;
        p->saved_sp = p->trapframe->sp;
        p->saved_gp = p->trapframe->gp;
        p->saved_tp = p->trapframe->tp;
        p->saved_t0 = p->trapframe->t0;
        p->saved_t1 = p->trapframe->t1;
        p->saved_t2 = p->trapframe->t2;
        p->saved_t3 = p->trapframe->t3;
        p->saved_t4 = p->trapframe->t4;
        p->saved_t5 = p->trapframe->t5;
        p->saved_t6 = p->trapframe->t6;
        p->saved_a0 = p->trapframe->a0;
        p->saved_a1 = p->trapframe->a1;
        p->saved_a2 = p->trapframe->a2;
        p->saved_a3 = p->trapframe->a3;
        p->saved_a4 = p->trapframe->a4;
        p->saved_a5 = p->trapframe->a5;
        p->saved_a6 = p->trapframe->a6;
        p->saved_a7 = p->trapframe->a7;
        p->saved_s0 = p->trapframe->s0;
        p->saved_s1 = p->trapframe->s1;
        p->saved_s2 = p->trapframe->s2;
        p->saved_s3 = p->trapframe->s3;
        p->saved_s4 = p->trapframe->s4;
        p->saved_s5 = p->trapframe->s5;
        p->saved_s6 = p->trapframe->s6;
        p->saved_s7 = p->trapframe->s7;
        p->saved_s8 = p->trapframe->s8;
        p->saved_s9 = p->trapframe->s9;
        p->saved_s10 = p->trapframe->s10;
        p->saved_s11 = p->trapframe->s11;
        p->trapframe->epc = p->handler;
  
        p->ticks = 0;
      }
      yield();
    }
  
    usertrapret();
  ```

**模块测试结果如下**：

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-28%2023.46.07.png" alt="截屏2022-08-28 23.46.07" style="zoom:50%;" />