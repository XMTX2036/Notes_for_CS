# LEC11 Scheduling

**Why support multiple tasks?**

- **time-sharing**: many users and/or many running programs
- **program structure**: prime number sieve
- **parallel speedup on multi-core hardware**.

*thread = an independent serial execution -- registers, pc, stack*

**two main strategies:**

- multiple CPUs, each CPU runs a different thread
- each CPU "switches" between threads, runs one at a time

**Thread can share memory or not**

- xv6 kernel threads: they share kernel memory (thus locks)
- xv6 user processes: one thread per process, so no sharing
- Linux: supports multiple threads sharing a user process's memory

**Other techniques for interleaving multiple tasks**

- look up event-driven programming
- state machines

*threads are usually the most convenient*

**线程设计**

- 如何在多CPU行交错执行多个线程
  - 如何透明地交错执行
  - 调度(Scheduling) ===> 选择接下来执行线程
  
- 在线程不执行时需要保存什么
  - registers, stack, memory
  
  - 跟踪每个线程的状态
    - RUNNING
    - RUNNABLE
    - SLEEPING
    
    ```C
    // Per-CPU process scheduler.
    // Each CPU calls scheduler() after setting itself up.
    // Scheduler never returns.  It loops, doing:
    //  - choose a process to run.
    //  - swtch to start running that process.
    //  - eventually that process transfers control
    //    via swtch back to the scheduler.
    void
    scheduler(void)
    {
      struct proc *p;
      struct cpu *c = mycpu();
      
      c->proc = 0;
      for(;;){
        // Avoid deadlock by ensuring that devices can interrupt.
        intr_on();
    
        for(p = proc; p < &proc[NPROC]; p++) {
          acquire(&p->lock);
          if(p->state == RUNNABLE) {
            // Switch to chosen process.  It is the process's job
            // to release its lock and then reacquire it
            // before jumping back to us.
            p->state = RUNNING;
            c->proc = p;
            swtch(&c->context, &p->context);
    
            // Process is done running for now.
            // It should have changed its p->state before coming back.
            c->proc = 0;
          }
          release(&p->lock);
        }
      }
    }
    ```
    
    线程调度函数`scheduler`，会对进程的state进行检测：
    
    - 若进程状态为RUNNABLE，则将其设为RUNNING，同时对进程的context进行保存、交换
  
- 如何处理计算绑定线程(compute-bound threads)

# LEC 12 Coordination

**No other lockes for `swtch`** ===> avoid the deadlock

**Coordination — Sleep/Wakeup**

e.g. Pipes ===> Disk Read ===> Wait()

- Busy wait

  while pipe buffer is empty {}

  then return the data

**Lost Wakeups**

```C
broken_sleep(chan);
p->state = SLEEPING;
p->chan = chan;
swtch();
```

```C
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    if(p != myproc()){
      acquire(&p->lock);
      if(p->state == SLEEPING && p->chan == chan) {
        p->state = RUNNABLE;
      }
      release(&p->lock);
    }
  }
}
```

**How to Terminate a Thread?**

*Problem:*

- thread X cannot just destory thread Y
  - what if Y is executing on another core?
  - what if Y hold locks?
  - what if Y is in the middle of a complex update to important data structures?
- a thread cannot free all of its own resources
  - its own stack is still using
  - struct context may need to call `swtch`

*2 ways to get rid of processes*

- `exit`

  - ordinary case: process voluntarily quits with exit() system call

  - some freeing in exit(), some in parent's wait()

  ```C
  void
  exit(int status)
  {
    struct proc *p = myproc();
  
    if(p == initproc)
      panic("init exiting");
  
    // Close all open files.
    for(int fd = 0; fd < NOFILE; fd++){
      if(p->ofile[fd]){
        struct file *f = p->ofile[fd];
        fileclose(f);
        p->ofile[fd] = 0;
      }
    }
  
    begin_op();
    iput(p->cwd);
    end_op();
    p->cwd = 0;
  
    acquire(&wait_lock);
  
    // Give any children to init.
    reparent(p);
  
    // Parent might be sleeping in wait().
    wakeup(p->parent);
    
    acquire(&p->lock);
  
    p->xstate = status;
    p->state = ZOMBIE;
  
    release(&wait_lock);
  
    // Jump into the scheduler, never to return.
    sched();
    panic("zombie exit");
  }
  ```

  State == ZOMBIE

  1.dying but not yet dead

  2.won't run again

  3.won't (yet) be reallocated by fork(), either swtch() to scheduler

- `kill`

  **may not be safe to forcibly terminate a process**

  - `kill()` sets `p->killed` flag, nothing else
  - the target process itself checks for p->killed and calls exit() itself
  - look for "if(p->killed) exit(-1);" in usertrap() no locks are held at that point

  *What if `kill` target is sleeping?* ==>might OK

  ```C
  // Kill the process with the given pid.
  // The victim won't exit until it tries to return
  // to user space (see usertrap() in trap.c).
  int
  kill(int pid)
  {
    struct proc *p;
  
    for(p = proc; p < &proc[NPROC]; p++){
      acquire(&p->lock);
      if(p->pid == pid){
        p->killed = 1;
        if(p->state == SLEEPING){
          // Wake process from sleep().
          p->state = RUNNABLE;
        }
        release(&p->lock);
        return 0;
      }
      release(&p->lock);
    }
    return -1;
  }
  ```

- `wait`

  ```C
  // Wait for a child process to exit and return its pid.
  // Return -1 if this process has no children.
  int
  wait(uint64 addr)
  {
    struct proc *np;
    int havekids, pid;
    struct proc *p = myproc();
  
    acquire(&wait_lock);
  
    for(;;){
      // Scan through table looking for exited children.
      havekids = 0;
      for(np = proc; np < &proc[NPROC]; np++){
        if(np->parent == p){
          // make sure the child isn't still in exit() or swtch().
          acquire(&np->lock);
  
          havekids = 1;
          if(np->state == ZOMBIE){
            // Found one.
            pid = np->pid;
            if(addr != 0 && copyout(p->pagetable, addr, (char *)&np->xstate,
                                    sizeof(np->xstate)) < 0) {
              release(&np->lock);
              release(&wait_lock);
              return -1;
            }
            freeproc(np);
            release(&np->lock);
            release(&wait_lock);
            return pid;
          }
          release(&np->lock);
        }
      }
  
      // No point waiting if we don't have any children.
      if(!havekids || p->killed){
        release(&wait_lock);
        return -1;
      }
      
      // Wait for a child to exit.
      sleep(p, &wait_lock);  //DOC: wait-sleep
    }
  }
  ```

  **wait() is not just for app convenience, but for O/S as well**