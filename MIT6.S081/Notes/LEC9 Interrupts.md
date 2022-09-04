# LEC9 Interrupts

硬件(Hardware)

驱动程序(`Driver`)是操作系统中管理特定设备的代码

- 配置设备硬件，告诉设备执行操作，处理由此产生的中断，并与可能等待设备I/O的进程交互
- 需要操作系统关注的设备通常可以配置为生成中断，这是一种`trap`

在xv6中, 有关中断的调度在`devintr`(`kernel/trap.c`:177).

**New issues/complications:**

- asynchronous
  - interrupts running process      
  - interrupt handler may not run in context of process who caused interrupt    
- programming devices
  - device can be difficult to program

- concurrency
  - devices and process run in parallel

进程的内核态中执行*top half*，中断时间中执行*bottom half*

**top half**

- 通过`read` 或 `write`这样的system call来进行调用的，从而能让此设备执行I/O操作
- 只有top half才能和用户进程进行交互

## Code: Console input

## Code: Console output

## Concurrency in drivers

consoleread和consoleintr中`acquire`的调用

===> 获得一个锁，该锁保护控制台驱动程序的数据结构免受并发访问。

**三个并发危险**：

- 不同CPU上的两个进程可能会同时调用consoleread；

- 当CPU已经在consoleread中执行时，硬件可能会要求CPU提供控制台（实际上为UART）中断；

- 当consoleread执行时，硬件可能会在不同的CPU上提供控制台中断

## Timer interrupts

xv6使用timer interrupts来维护其时钟，并使其能够在计算绑定(`compute bound`)的进程之间切换；`usertrap`以及`kerneltrap`中调用的`yield`造成进程切换；

RISC-V要求timer interrupts在M Mode(RISC-V executes without paging)中被处理

- `PLIC`: Platform-Level Interrupt Controller
- `CLINT`: Core-Local Interruptor

**RISC-V对中断的支持**

- `sie` --- 中断启用寄存器
  - 每个软件中断、外部中断、定时器中断一位

- `sstatus` --- 状态寄存器
  - 一位启用中断

- `sip` --- 中断挂起寄存器

- `scause` --- 原因寄存器

- `stvec` --- trap向量寄存器

- `mdeleg` --- machine delegate register

由于timer interrupt可能在任意时间点发生，包括kernel在执行关键的操作时，无法强制关闭定时器中断，因此timer interrupt的发生不能够影响这些被中断的操作:

**解决措施**：timer interrupt handler使CPU产生并随后立即返回一个软件中断(software interrupt)，其可以通过正常的trap机制被送到kernel中，可以被kernel禁用