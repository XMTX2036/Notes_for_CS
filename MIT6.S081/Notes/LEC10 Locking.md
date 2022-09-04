# LEC10 Locking

并发控制(`Concurrency Control`) ===> 使用锁进行约束

锁提供了一种**互斥机制**，一段时间内只有一个CPU才能拥有这个锁，如果一个锁和一个被共享的数据结构联系起来，那么这个数据结构一次只能被一个CPU使用

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-03%2023.27.40.png" alt="截屏2022-09-03 23.27.40" style="zoom:50%;" />

## Race conditions

freelist由多个CPU共享

**一个内存地址同时被至少一个写入操作访问，会造成bug**

===> *executed in isolation*

```C
struct element *list = 0;
struct lock listlock;

void push (int data) {
    struct element *l;
    l = malloc(sizeof *l);
    l->data = data;
    acquire(&listlock);
    // critical section
    l->next = list;
    list = l;
    release(&listlock);
}
```

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-03%2023.30.37.png" alt="截屏2022-09-03 23.30.37" style="zoom:50%;" />

## Code: Locks

xv6有两种锁：`spinlocks`和`sleep-locks`

```C
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
```

如果`locked=0`，则未上锁；反之，若`locked`非零，则上锁(lock held)

```C
void
acquire(struct spinlock *lk) // does not work!
{
  for(;;) {
    // 多核情况下会出现问题，需要使lk->locked == 0的判断与赋值为1成为一个原子性操作
    if(lk->locked == 0) {
      lk->locked = 1;
			break; 
    }
	} 
}
```

有效的实现：

```C
void
acquire(struct spinlock *lk)
{
  push_off(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    panic("acquire");

  // On RISC-V, sync_lock_test_and_set turns into an atomic swap:
  //   a5 = 1
  //   s1 = &lk->locked
  //   amoswap.w.aq a5, a5, (s1)
  while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen strictly after the lock is acquired.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize();

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu();
}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  lk->cpu = 0;

  // Tell the C compiler and the CPU to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other CPUs before the lock is released,
  // and that loads in the critical section occur strictly before
  // the lock is released.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code doesn't use a C assignment, since the C standard
  // implies that an assignment might be implemented with
  // multiple store instructions.
  // On RISC-V, sync_lock_release turns into an atomic swap:
  //   s1 = &lk->locked
  //   amoswap.w zero, zero, (s1)
  __sync_lock_release(&lk->locked);

  pop_off();
}
```

关键 ===> `RISC-V的amoswap指令`

```assembly
# 将a内存地址中的内容和r寄存器中的内容互换
amoswap r, a
```

在`acquire`中，通过一个对`amoswap`的包装函数`__sync_lock_test_and_set(&lk->locked, 1)`来实现这个原子操作，其返回值是`lk->locked`的旧的值(被换下来的值)

`release`是`acquire`的反向操作，先将`lk->cpu`清零，随后调用

```C
__sync_lock_release(&lk->locked);
// s1 = &lk->locked
// amoswap.w zero, zero, (s1)
```

## Code: Using locks

```C
lock l
acquire(l);
x = x + 1;  // critical section
release(l);
```

**保守规则**:任何时候两个线程使用一个内存位置，并且至少有一个是写入，除非持有正确的锁，否则不可接触共享数据

## Deadlock and lock ordering

一个代码块需要多个锁，那么所有代码路径都必须以相同的顺序获取这些锁 ===> 否则会导致**死锁**

Cause of Deadlock

- 加锁的循序
- 锁与程序逻辑结构的关系

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-03%2023.21.51.png" alt="截屏2022-09-03 23.21.51" style="zoom:50%;" />

## Re-entrant locks

Re-entrant locks/ *recursive locks* <=== possibly avoid some deadlocks and lock-ordering challenges

事实上，`re-entrant lock`使得并发控制更加难以处理

xv6使用`non-re-entrant lock`

## Locks and interrupt handlers

spinlock与中断的相互作用会带来一些潜在危险 ===> 死锁

为了避免死锁等情况，如果中断处理程序使用`spinlock`，CPU绝不能在启用中断的情况下保持该锁；

**xv6更保守**：

- 当CPU获得任何锁时，xv6总是禁用该CPU上的中断；
- 中断可能仍然发生在其他CPU上，因此中断的获取可以等待线程释放旋转锁；
- 锁与中断不在同一CPU上发生；
- 当CPU没有旋转锁时，xv6会重新启用中断

## Instruction and memory ordering

许多编译器和CPU不按顺序执行代码，以实现更高的性能。如果指令需要多个周期才能完成，CPU可能会尽早发布指令，以便它与其他指令重叠并避免CPU `stalls`

编译器和CPU在重新排序时遵循规则，以确保它们不会更改正确编写的串行代码的结果

## Sleep locks

`spinlock`的两个缺点：

- 如果一个进程拥有一个锁很长时间，另外一个企图acquire的进程将一直等待
- 当一个进程拥有锁的时候，不允许把当前使用的CPU资源切换给其他线程，否则可能导致第二个线程也acquire这个线程，然后一直无法切回到原来的线程，无法release锁，从而导致死锁。

`sleep-locks`，可以在试图`acquire`一个被拥有的锁时`yield` CPU

spin-lock适合短时间的关键步骤，sleep-lock适合长时间的锁。
