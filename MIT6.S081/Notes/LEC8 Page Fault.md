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