# LEC6 Isolation & System Call Entry/Exit

**三类使CPU暂停(set aside)正常指令执行的事件(event)**

- 系统调用
- 异常(exception)
- (设备)中断(device interrupt)

在xv6中，使用traps作为上述三种情况的一个通用术语，**traps发生执行的任何代码都需要恢复，且不需要意识到任何特殊的事情发生** ===> traps是**透明的(transparent)**

traps的顺序：

- 强制将控制权转移到内核，内核保存寄存器以及其他状态量 ===> 使得执行可恢复
- 内核可执行适当的处理代码(handle code)
- 内核从trap中恢复之前保存的状态
- 原代码从中断处恢复执行

所有的traps都在内核中被处理，不会被传递到用户代码

xv6陷阱处理分**4**个阶段进行：

- RISC-V CPU采取的硬件操作，
- 为内核C代码准备道路的一些程序集指令，
- 决定如何处理陷阱的C函数，
- 以及系统调用或设备驱动程序服务例程。

> Kernel code (assembler or C) that processes a trap is often called a *handler*; the first handler instructions are usually written in assembler (rather than C) and are sometimes called a *vector*.

## RISC-V trap machinery

最重要的几个寄存器(与supervisor mode相关，它们在user mode下不可被读写)

- stvec: 内核在此处写入trap处理程序的地址；RISC-V跳转到stvec中的地址来处理trap
- sepc: 当trap发生时，RISC-V会将PC保存在sepc中(随后PC的值将赋值为stvec中的值)。sret(从trap返回)指令将sepc复制到pc，内核可以写入sepc来控制sret的位置
- scause: 记录trap发生的理由
- sscratch：内核在这里存有一个值，该值在trap处理的一开始就被使用
- sstatus：sstatus中的SIE位控制设备中断是否已启用。如果内核清除SIE，RISC-V将推迟设备中断，直到内核设置SIE。SPP位指示陷阱是来自用户模式还是主管模式，并控制了sret返回的模式

在machine mode中有一套与上述类似的寄存器组，但是xv6中仅用于处理timer interrupts的特殊情况

Traps(除timer interrupt)发生，RISC-V执行下列内容：

- 如果trap为设备中断，而sstatus的SIE位未被设置，则下列的操作都不会被执行
- 通过在sstatus中清除SIE位来禁用中断
- 将当前PC复制到sepc
- 将当前模式(user/supervisor)存至sstatus的SPP位
- 设置scause ===> 产生traps的原因
- 将模式设置为supervisor
- 将stvec中值拷贝值PC
- 在新的PC处开始执行代码

## Traps from user space

User space中的trap路径：

- uservec

  `uservec`开始时32个寄存器的值均为trap前的，会保存在内存中 ===> 需要一个寄存器来保存旧寄存器内容的地址；使用`sscratch`，将`a0`的值存在`sscratch`中(交换两个寄存器内容)；交换之前的`sscratch`中是指向user process的`trapframe`的地址，`trapframe`中预留了保存所有32个寄存器的空间。`p->trapframe`保存了每个进程的`TRAPFRAME`的物理空间从而让kernel页表也可以访问该进程的trapframe；随后，`uservec`可以通过`a0`把所有当前寄存器的值**保存到trapframe中**。由于当前进程的trapframe已经保存了当前进程的kernel stack、当前CPU的hartid、`usertrap`的地址、kernel page table的地址等，`uservec`需要获取这些值，然后切换到kernel pagetable，调用`usertrap`

- usertrap

  `usertrap`主要是判断trap产生的原因并进行处理，然后返回。因为当前已经在kernel里了，所以这时候如果再发生trap，应该交给`kernelvec`处理，因此要把`stvec`切换为`kernelvec`。如果trap是一个system call，那么`syscall`将被调用，如果是设备中断，调用`devintr`，否则就是一个exception，kernel将杀死这个出现错误的进程

- usertrapret

  调用`usertrapret()`，这个函数将把`stvec`指向`uservec`，user space中再出现trap时跳转至`uservec`，同时设置`p->trapframe`的一些值为下一次trap作准备，比如设置`p->trapframe->kernel_sp = p->kstack + PGSIZE`；清除`SPP`为从而使得调用`sret`后能够回到user mode。设置回到user space后的program counter为`p->trapframe->epc`，最后调用跳转到TRAMPOLINE页上的`userret`回到trampoline.S，加载user page table

- userret

  `userret`被`userrapret`调用返回时a0寄存器中保存了TRAPFRAME，因此可以通过这个TRAPFRAME地址来恢复之前所有寄存器的值(包括a0)，最后把TRAPFRAME保存在sscratch中，用`sret`回到user space

当trap发生时，RISC-V硬件不会切换页表 ===> 

- xv6的trap处理程序需要切换至内核页表
- 内核页表必须有一个指向handler的映射(通过stvec寄存器)

**xv6通过一个trampoline页满足上述需求**(定义在user space中 ===> PTE_U)，**traps可以在此处开始执行** ===> trampoline页在内核地址空间中映射在相同地址，trap可以在切换到内核页表中继续执行

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-28%2013.53.36.png" alt="截屏2022-08-28 13.53.36" style="zoom:50%;" />

## Code: Calling system calls

exec的参数放到寄存器`a0`以及`a1`中，将对应的syscall的syscall number放至`a7`中，`ecall`指令捕获到内核中，并导致uservec、usertrap，然后syscall执行；kernel trap把寄存器的值保存在当前进程的trapframe中；syscall将把trapframe中的a7寄存器保存的值提取出来，索引到`syscalls`这个函数数列中查找对应的syscall种类，并进行调用，然后把返回值放置在`p->trapframe->a0`中，如果执行失败，就返回-1

## Code: System call arguments

argint/ argaddr/ argfd

## Traps from kernel space

当kernel space中发生trap时，`stvec`是指向`kernelvec`的汇编代码的。`kernelvec`将寄存器的值保存在被中断的kernel thread的`stack`里而不是trapframe里，这样当trap需要切换kernel thread时，再切回来之后还可以从原先的thread栈里找到之前的寄存器值。

寄存器的值保存完后，跳转到`kerneltrap`(trap handler)。`kerneltrap`可以对设备中断和exception这两种trap进行处理。

- 设备中断，调用`devintr`进行处理
- exception，panic
- timer interrupt，调用`yield`让其他kernel thread运行

最后返回到`kernelvec`中，`kernelvec`将保存的寄存器值从堆栈中弹出，执行`sret`，将`sepc`复制到`pc`来执行之前被打断的kernel code

**Page-fault exceptions would be mentioned in the next lecture!**