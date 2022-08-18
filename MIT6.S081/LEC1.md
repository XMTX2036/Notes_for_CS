# LEC1 Introduction and examples

## 0. Preparation

### 0.1 Read Chapter1(Operating system interfaces)

- The job of an operating system is to share a computer among multiple programs and to provide a more useful set of services than the hardware alone supports
- An operating system manages and abstracts the low-level hardware
- The trick in resolving this tension(interface) is to design interfaces that rely on a few mechanisms that can be combined to provide much generality.

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-15 17.34.26.png" alt="截屏2022-07-15 17.34.26" style="zoom:50%;" />

- Each running program called a *process* has memory containing instructions, data, and a stack

- A computer with only a single kernel but many processes
- A process alternates between executing in user space and kernel space

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-16 11.15.25.png" alt="截屏2022-07-16 11.15.25" style="zoom:50%;" />

- The shell is an ordinary program that reads commands from the user and executes them. The fact that the shell is a user program, and not part of the kernel, illustrates the power of the system call interface: there is nothing special about the shell

**1.1 Processes and memory**

- In xv6, a process consists of user-space memory and per-process state private to the kernel
- Time-shares process
- Syscalls: **fork** & **exec**

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid = fork();
    if(pid > 0) {
        printf("parent: child pid = %d\n", pid);
        pid = wait((int *)0);
        printf("child process is done!\n");
    } else if(pid == 0) {
        printf("child process is existing!\n");
        exit(0);
    } else {
        printf("fork error!\n");
    }

    return 0;
}
```

**1.2 I/O and File descriptors**

- File Descriptors
  - file descriptors 0 ===> standard input
  - file descriptors 1 ===> standard output
  - file descriptors 2 ===> standard error
- Syscalls:**read** & **write**

**1.3 Pipes**(Offer a way for communicate between processes)

A pipe is a small kernel buffer exposed to processes as a pair of file descriptors, one for reading
and one for writing.

**1.4 File system**

**1.5 Real world**

## 1. OS Purposes

- Abstarct Hardware
- Multiplex
- Isolation
- Sharing
- Security(Access system)
- Performance
- Range of OS

## 2. OS Organization

**User**  			 vi		cc		sh

**Kernel** 		  File_System

​						Processes/MEM Alloc/Access Ctrl

**Hardware** 	CPU RAM DISK NET

## 3. API-kernel(System Call)

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-07-16 11.15.25.png" alt="截屏2022-07-16 11.15.25" style="zoom:50%;" />

```C
fd = open("out", 1);
write(fd, "hello\n", 6);
pid = fork();
```

## 4. Why OS Hard & Interesting?

- Unforgiving
- Tensions
  - Efficient -- Abstract
  - Powerful -- Simple
  - Flexible -- Secure

## 5. System Call Cont.

`copy.c`

`open.c`

`fork.c`

`exec.c`

`forkexec.c`

`redirect.c`

## Lab1 Xv6 and Unix utilities

### Boot xv6

### Sleep

直接调用库函数`sleep()`即可

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
    if(argc < 2){
		fprintf(2, "Usage: sleep time...\n");
		exit(1);
	}
	sleep(atoi(argv[1]));
	exit(0);
}
```

### Pingpong

**Hints：**

- pipe：创建一个管道，将读写文件的描述存在p[0]以及p[1]
- fork：创建一个进程，返回孩子进程的PID;**在子进程中，返回值为0**
- read：从pipe中读
- write：写入pipe中

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int 
main(int argc, char *argv[])
{
    int pid;
    int n;
    char buf[2];
    int p[2];

    pipe(p);
    pid = fork();

    if (pid < 0) {
        exit(1);
    }else if (pid == 0) {
        n = read(p[0], buf, 1);
        if (n < 0) {
            fprintf(2, "read error in child process\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        close(p[0]);
        n = write(p[1], buf, 1);
        if (n < 0) {
            fprintf(2, "write error in child process\n");
            exit(1);
        }
        close(p[1]);
        exit(0);
    }else {
        buf[1] = '\0';
        n = write(p[1], buf, 1);
        if (n < 0) {
            fprintf(2, "write error in parent process\n");
            exit(1);
        }
        close(p[1]);
        n = read(p[0], buf, 1);
        if (n < 0) {
            fprintf(2, "read error in parent process\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());
        close(p[0]);
        exit(0);
    }
}
```

### Primes

### Find

### Xargs

