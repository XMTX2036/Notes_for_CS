# LEC8 Page Fault

## Lecture Note

**Plan**:

- Better performance/efficiency
- New features
  - memory-mapped files

### **Virtual Memory**

- Isolation
- A level-of-indirection(virtual memory ===> physical memory)
  - Shared trampoline page
  - guard page

### Page Fault

#### Key idea: change pgtbl on page fault

**a form of trap** ===> like a system call

xv6 panics on page fault

**Instead ===> update pgtbl & restart instruction**

#### RISC-V page faults

3 of 16 exceptions are related to paging

- VAddr that caused the fault

  See `stval` register; page faults set it to the fault addr.

- The type of violation that caused the fault

  See `scause` register

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-31%2016.00.03.png" alt="截屏2022-08-31 16.00.03" style="zoom:50%;" />

- The instruction and mode where the fault occurred

  - User IP: `tf->epc`
  - U/K mode: implicit in `usertrap/kerneltrap`

### Lazy/on-demand page allocation

- `sbrk()` is old fashioned

  applications often ask for memory they need

- modern OSes allocate memory lazily ===> **may use less memory**

  **Plan**:

  - allocate physical memory when application need it
  - adjust `p->sz` on sbrk, but don't allocate
  - when application uses that memory, it will result in page fault
  - on pagefault allocate memory
  - resume at the fault instruction

### One zero-filled page(zero fill on demand)

- Applications often have large part of memory that must zero global arrays, etc.

  `.bbs` segment

  ===> kernel must often fill one page with 0 `memset`

### Copy-On-Write(COW) fork

xv6 `fork` copies all pages from parent but fork is often immediately followed by `exec`

**share address space between *parent* and *child***

modify `fork()` to map pages `cow`

- use extra available system bits(RSW) in PTEs
- on page fault, make copy of page and map it r/w

### Demanding page

`exec` loads the whole file into memory ===> too expensive

**load pages from the file on demand**

- allocate PTEs, but mark them on-demand
- on fault, read the page in from the file and updte PTE
- need to keep some meta info about the location of the page on disk
  - VMA(virtual memory area)

**How to deal with the situation that file is larger than physical memory?**

===> **use virtual memory larger than physical memory**(LRU)

### Memory-Mapped files

Allow access to files using load and store can easily r/w part of a file

### Shared virtual memory

Allow processes on different machines to share virtual memory

- copy pages that are only read
- invalidate copies on write

### TLB management

RISC-V allow more sophisticated plans

- PTE_G: global TLB bits

- ASID numbers

- Large pages

  2MB and 1GB

## Book & Code Note

xv6的exception处理：

- 若发生在user空间中，内核直接杀死故障进程
- 若发生在内核空间中，内核会直接崩溃

现实应用中的OS处理方式更加有趣

===> use page faults to implement COW fork

fork过程中，使父进程和子进程可以**共用父进程的物理内存**则会更有效率

- 直接分享物理内存会造成进程之间的相互扰乱
- 可以通过使用页表许可以及page faults来安全地共享物理内存
- 当使用虚拟地址时，页表中没有映射，或者具有PTE_V标志清晰的映射，或者权限位（PTE_R、PTE_W、PTE_X、PTE_U）禁止尝试操作的虚拟地址时，CPU会引发page fault

RISC-V 3 types of page faults ===> `scause` & `stval`

- load page fault
- store page fault
- instruction page fault

**基本步骤/计划**

- 父进程、子进程都映射为只可读(清除PTE_W标志位)
- 若父子进程都要对页面进行写操作，则CPU发出page fault的异常
- 内核的trap处理程序通过分配新的物理内存页面并将page fault地址映射到的物理页面复制到其中来响应
- 内核更改fault进程页面表中的相关PTE，以指向copy并允许r/w，然后根据导致故障的指令恢复故障过程

**优化**

若一个进程出现store page fault，并且物理页面仅从该进程的页面表中引用，则不需要进行copy

**Lazy Allocation**

**过程**

- 当一个应用通过调用`sbrk`想获得更多内存时，内核注意/记录内存的增大，但不会分配物理内存 & 为新的VAddr创建PTE
- 在其中一个新地址的页面故障中，内核分配一页物理内存并将其映射到页表中

**优点**

- 使分配内存的开销随时间分摊

- 操作系统可以通过为每个page fault分配一批连续的页面，而不是一页，并通过专门化此类页面故障的内核输入/退出代码来降低这一成本。

**其他**

- demand paging
- paging to disk