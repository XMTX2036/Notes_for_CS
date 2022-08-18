# LEC2 C and gdb

# LEC3 OS organization and system calls

## 0. Preparation

## 0.1 Chapter2

- A key requirement for an OS is to support several activities at once.
- The OS must **time-share** the recourses of the computer among these processes

## 0.2 xv6 code

Topics:

- Isolation
- Kernel/User mode
- System call
- xv6

## 1.Unix interface

Abstract the hardware resources

- Processes: instead CPU

- Exec: instead of MEM
- Files: instead of disk block

## 2. OS should be defensive

- App cannot crash the OS

- App cannot break out of its isolation

Strong Isolation between apps and OS

**Hardware Support:**

- User/Kernel Mode

  Privileged instructions --> Kernel Mode

  - Set up page table 
  - Disabling clock
  - Interrupts

  Unprivileged instructions --> User Mode

  - Add
  - Sub
  - Jmp
  - Branch

- Virtual Memory(VM)

  Page table: Virtual addr. --> Physical addr.

  Process its own page table 

  MEM Isolation

## 3. Entering Kernel

ecall --> system call numbers

entry point

**U**									|								**K**

fork() ecall sys_fork	 \						/	fork

​										| --> syscall 

write() ecall sys_write  /						\	write

**Kernel = trusted computing base(TCB)**

- Kernel must have no bugs
- Kernel must treat processes as malicious

All these are for **security**

## 4. Micro-Kernel Design

Microkernel

- Kernel is small --> fewer bugs 
- It's difficult to achieve high performance than monolithic kernel

Monolithic Kernel

## 5. xv6

- kernel
- user
- mkfs(file system)