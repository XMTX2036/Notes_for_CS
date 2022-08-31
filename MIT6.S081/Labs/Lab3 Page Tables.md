# Lab3 Page Tables

在本实验中，需要对页表进行修改以加快某些系统调用并完成检测被访问页的系统调用

## Speed up system calls

通过在用户空间与内核之间共享只读区域中的数据以加快系统调用

创建每个进程时，在`USYSCALL`(memlayout.h 中定义的 VA)映射一个只读页面。在这个页面的开始，存储一个struct ussyscall(也在`memlayout.h`中定义)，并初始化它来存储当前进程的PID

- 在`struct proc`中增加对于usyscall的声明

  ```C
  struct proc{
    // ...
    struct usyscall *ucall
    // ...
  }
  ```

- 在`static struct proc *allocproc(void)`中增加对于usyscall的初始化以及存储当前PID

  ```C
  static struct proc*
  allocproc(void)
  {
    struct proc *p;
  
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == UNUSED) {
        goto found;
      } else {
        release(&p->lock);
      }
    }
    return 0;
  
  found:
    p->pid = allocpid();
    p->state = USED;
  
    // Allocate a trapframe page.
    if((p->trapframe = (struct trapframe *)kalloc()) == 0){
      freeproc(p);
      release(&p->lock);
      return 0;
    }
  
    if ((p->ucall = (struct usyscall*)kalloc()) == 0){
  	  freeproc(p);
  	  release(&p->lock);
  	  return 0;
    }
  
    // An empty user page table.
    p->pagetable = proc_pagetable(p);
    if(p->pagetable == 0){
      freeproc(p);
      release(&p->lock);
      return 0;
    }
  #ifdef LAB_PGTBL
    p->ucall->pid = p->pid;
  #endif
    // Set up new context to start executing at forkret,
    // which returns to user space.
    memset(&p->context, 0, sizeof(p->context));
    p->context.ra = (uint64)forkret;
    p->context.sp = p->kstack + PGSIZE;
  
    return p;
  }
  ```

  **注意**：

  由于usyscall结构体在`memlayout.h`中的定义如下：

  ```C
  #ifdef LAB_PGTBL
  #define USYSCALL (TRAPFRAME - PGSIZE)
  
  struct usyscall {
    int pid;  // Process ID
  };
  #endif
  ```

  因此，在调用usyscall结构体相关的操作时，也应当包含`#ifdef`以及`#endif`

- `proc_pagetable()`中增加对于只可读页面的映射

  ```C
  // Create a user page table for a given process,
  // with no user memory, but with trampoline pages.
  pagetable_t
  proc_pagetable(struct proc *p)
  {
    pagetable_t pagetable;
  
    // An empty page table.
    pagetable = uvmcreate();
    if(pagetable == 0)
      return 0;
  
    // map the trampoline code (for system call return)
    // at the highest user virtual address.
    // only the supervisor uses it, on the way
    // to/from user space, so not PTE_U.
    if(mappages(pagetable, TRAMPOLINE, PGSIZE,
                (uint64)trampoline, PTE_R | PTE_X) < 0){
      uvmfree(pagetable, 0);
      return 0;
    }
  
    // map the trapframe just below TRAMPOLINE, for trampoline.S.
    if(mappages(pagetable, TRAPFRAME, PGSIZE,
                (uint64)(p->trapframe), PTE_R | PTE_W) < 0){
      uvmunmap(pagetable, TRAMPOLINE, 1, 0);
      uvmfree(pagetable, 0);
      return 0;
    }
  #ifdef LAB_PGTBL
    // map the read-only page
    if(mappages(pagetable, USYSCALL, PGSIZE, 
      (uint64)(p->ucall), PTE_U | PTE_R) < 0) {
        uvmfree(pagetable, 0);
  
        return 0;
    }
  #endif  
  
    return pagetable;
  }
  ```

- 在`static void freeproc(struct proc *p)`中对上述页进行free操作

  ```C
  static void
  freeproc(struct proc *p)
  {
    if(p->trapframe)
      kfree((void*)p->trapframe);
    p->trapframe = 0;
    if(p->pagetable)
      proc_freepagetable(p->pagetable, p->sz);
    // added
    if(p->ucall) 
      kfree((void *)p->ucall);
    p->ucall = 0;	// added
    p->pagetable = 0;
    p->sz = 0;
    p->pid = 0;
    p->parent = 0;
    p->name[0] = 0;
    p->chan = 0;
    p->killed = 0;
    p->xstate = 0;
    p->state = UNUSED;
  }
  ```

- `proc_freepagetable`在中，free该进程的readonly页

  ```C
  void proc_freepagetable(pagetable_t pagetable, uint64 sz) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmunmap(pagetable, TRAPFRAME, 1, 0);
    uvmunmap(pagetable, USYSCALL, 1, 0);
    uvmfree(pagetable, sz);
  }
  ```

## Print a page table

可视化的打印页表

学习`vm.c`中的`freewalk`函数，递归逐层打印页表内容即可

```C
void
printwalk(pagetable_t pagetable, int depth)
{
  for(int i = 0; i < 512; i++){
    pte_t pte = pagetable[i];
    if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this pte points to a valid lower level page table
      uint64 child = PTE2PA(pte);
      for (int j=0; j<=depth; j++) {
        printf("..");
        if ((j+1) <= depth) printf(" ");
      }
      printf("%d: pte %p pa %p\n", i, pte, child);
      printwalk((pagetable_t)child, depth+1);
    }
    else if (pte & PTE_V) { // the leaf page
      uint64 child = PTE2PA(pte);
      printf(".. .. ..%d: pte %p pa %p\n", i, pte, child);
    }
  }
}
   
// print the page table
void vmprint(pagetable_t pagetable)
{
  printf("page table %p\n", pagetable);
  printwalk(pagetable, 0);
}
```

**Note**:

- 在defs.h中添加vmprint的函数声明
- 在exec.c中按照题目提示添加`if(p->pid == 1) vmprint(p->pagetable);`语句

## Detecting which pages have been accessed

实现`pgaccess()`，这是一个报告哪些页面已被访问的系统调用。系统调用需要三个参数。首先，它需要检查第一个用户页面的起始虚拟地址。其次，它需要检查页数。最后，它需要一个用户地址到一个缓冲区，以将结果存储到一个位掩码(一种每页使用一个位并且第一页对应于最低有效位的数据结构)

```C
#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  const int MAX_PGNUM = 64;
  // lab pgtbl: your code here.
  uint64 va_start;
  int page_num;
  uint64 buffer;
  // check the para.s
  if (argaddr(0, &va_start) < 0 || argint(1, &page_num) < 0 || argaddr(2, &buffer) < 0) return -1;
  
  page_num = page_num > MAX_PGNUM ? MAX_PGNUM : page_num;
  uint64 mask = 0;
  struct proc *p = myproc();
  pte_t *pte;
  for (int i=0; i<page_num; i++){
    pte = walk(p->pagetable, va_start+PGSIZE*i, 0);
    if(pte && (*pte & PTE_A)){
      mask |= (1 << i);
      *pte &= ~PTE_A; // clear the sign bits
    }
  }

  if (copyout(p->pagetable, buffer, (char *)&mask, sizeof(uint64))) return -1;
  return 0;
}
#endif
```

**Note**:

- 在`kernel/riscv.h`定义`PTE_A` (代表the access bit). 

![截屏2022-08-26 15.45.30](https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-26%2015.45.30.png)

- 设置一个检查页面数量的上限(`const int MAX_PGACCESS_PAGE_NUMBER = 64;`)
- 请务必在检查是否设置后清除 PTE_A