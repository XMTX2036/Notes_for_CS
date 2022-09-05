# Xv6 Code Reading

## OS Organization and System Calls

### xv6启动第一个进程

- RISC-V计算机开机时，自行初始化并运行一个引导加载程序(存储在ROM中)，引导加载程序将xv6内核加载到内存中

- 然后，在`machine mode`下，CPU从`_entry`(kernel/entry.S:7)开始执行xv6,RISC-V从禁用分页硬件开始：虚拟地址直接映射到物理地址

- `_entry`设置堆栈，在文件`start.c`中声明初始堆栈stack0的空间

- `start`执行某些配置(仅在`machine mode`中可执行)，随后切换至`supervisor mode` ===> RISC-V `mret`， 随后PC切换至`kernel/main.c`

- `main`先对一些设备和子系统进行初始化，然后调用`kernel/proc.c`中定义的`userinit`来创建第一个用户进程，这个进程执行了一个`initcode.S`的汇编程序，这个汇编程序调用了`exec`这个system call来执行`/init`，重新进入kernel`exec`将当前进程的内存和寄存器替换为一个新的程序(`/init`)，当kernel执行完毕`exec`指定的程序后，回到`/init`进程

  ```C
  // Set up first user process.
  void
  userinit(void)
  {
    struct proc *p;
  
    p = allocproc();
    initproc = p;
    
    // allocate one user page and copy init's instructions
    // and data into it.
    uvminit(p->pagetable, initcode, sizeof(initcode));
    p->sz = PGSIZE;
  
    // prepare for the very first "return" from kernel to user.
    p->trapframe->epc = 0;      // user program counter
    p->trapframe->sp = PGSIZE;  // user stack pointer
  
    safestrcpy(p->name, "initcode", sizeof(p->name));
    p->cwd = namei("/");
  
    p->state = RUNNABLE;
  
    release(&p->lock);
  }
  ```

- `/init`(`user/init.c`)创建了一个新的console device以文件描述符`stdin`,`stdout`,`stderr`打开，然后在console device中开启了一个shell进程，整个操作系统启动

### `kernel/proc.h` 

**Caller/Callee Saved Register**

*Def*：

- **caller saved register**(aka **volatile** registers，or **call-clobbered**)

  用于保存不需要在调用之间保留的临时数量

- **callee saved register**(aka **non-volatile** registers，or **call-preserved**)

  用于保存应该在调用之间保留的长期值。

*Context(上下文)* ===> 一组callee saved register

```C
// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};
```

**进程状态**

```C
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
```

- UNUSED(=0)
- USED(=1)
- SLEEPING(=2)
- RUNNABLE(=3)
- RUNNING(=4)
- ZOMBIE(=5)

**CPU**

```C
// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU]; // CPU数量
```

**trapframe**

`trapframe`存储在出现trap(exception & device interrupt & timer interrupt)期间保存的寄存器组，因此使用 `trapframe`使得程序在返回后可以继续执行

```C
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};
```

- `satp`寄存器

  kernel pgtbl

- `epc`

  当前的PC(`Program Counter`)值，记录当前执行的指令

**struct proc**

```C
// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state;        // Process state
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  int xstate;                  // Exit status to be returned to parent's wait
  int pid;                     // Process ID

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)

  int mask;
};
```

记录每个进程的状态

- `pid`

  进程号

- `sz`

  该进程的内存大小(以byte为单位)

- `pagetable`

  用户页表

  ===> satp寄存器 — 内核页表

- `kstack`

  内核堆栈的VA

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-05%2013.40.05.png" alt="截屏2022-09-05 13.40.05" style="zoom:50%;" />

### `kernel/defs.h`

所有函数 & 结构体的**声明**

### `kernel/entry.S`

```assembly
				# qemu -kernel loads the kernel at 0x80000000
        # and causes each CPU to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
				# set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + (hartid * 4096)
        la sp, stack0
        li a0, 1024*4
				csrr a1, mhartid
        addi a1, a1, 1
        mul a0, a0, a1
        add sp, sp, a0
				# jump to start() in start.c
        call start
spin:
        j spin
```

- 内核的起始地址位于`0x80000000`

- 每个CPU都有一个对应的stack，因此在`start.c`中的`stack0`定义如下

  ```C
  // entry.S needs one stack per CPU.
  __attribute__ ((aligned (16))) char stack0[4096 * NCPU];
  ```

- `start()`

  ```C
  // entry.S jumps here in machine mode on stack0.
  void
  start()
  {
    // set M Previous Privilege mode to Supervisor, for mret.
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);
  
    // set M Exception Program Counter to main, for mret.
    // requires gcc -mcmodel=medany
    w_mepc((uint64)main);
  
    // disable paging for now.
    w_satp(0);
  
    // delegate all interrupts and exceptions to supervisor mode.
    w_medeleg(0xffff);
    w_mideleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  
    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);
  
    // ask for clock interrupts.
    timerinit();
  
    // keep each CPU's hartid in its tp register, for cpuid().
    int id = r_mhartid();
    w_tp(id);
  
    // switch to supervisor mode and jump to main().
    asm volatile("mret");
  }
  ```

  执行一些仅能在`machine mode`下进行的配置，然后切换至`supervisor mode`(通过RISC-V命令`mret`)

  - `mstatus`，获得`r_mstatus`当前寄存器内容 & `w_mstatus`设置`mstatus`的内容
  - 设置返回地址，写入`mepc`寄存器
  - 禁止使用VA translation ===> 将0写入satp寄存器
  - 将exception与设备中断均设置在`supervisor mode`

### `kernel/main.c`

```C
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
```

- `start()`跳转至`main.c`，进行一系列的初始化`xxxinit()`

### `user/initcode.S`

```C
# Initial process that execs /init.
# This code runs in user space.

#include "syscall.h"

# exec(init, argv)
.globl start
start:
        la a0, init
        la a1, argv
        li a7, SYS_exec
        ecall

# for(;;) exit();
exit:
        li a7, SYS_exit
        ecall
        jal exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .long init
  .long 0
```

