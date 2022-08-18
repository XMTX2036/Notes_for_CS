# Lab1 Xv6 and Unix utilities

## Boot xv6(easy)

按照实验指导，使用QEMU运行xv6即可

```shell
make qemu
```

Note：通过ctrl-a + x退出QEMU

## Sleep(easy)

为xv6实现一个Unix程序sleep，命令格式如下：

```shell
sleep 10
```

**具体实现**：直接调用sleep函数即可

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
    if(argc != 2){
		fprintf(2, "Usage: sleep seconds\n");
		exit(1);
	}
	sleep(atoi(argv[1]));
	exit(0);
}
```

## Pingpong(easy)

编写一个程序，使用 UNIX 系统调用通过一对管道在两个进程之间“乒乓”一个字节，一个管道用于每个方向。父母应该向孩子发送一个字节；子进程应该打印“<pid>: received ping”，其中 <pid> 是它的进程 ID，将管道上的字节写入父进程，然后退出；父母应该从孩子那里读取字节，打印“<pid>: received pong”，然后退出

**具体实现**：通过pipe创建管道，fork创建子进程

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

Note: 

- fprintf第一个参数FILE *stream，值为2表示sdterr

- read函数原型`read(int fd, char *buf, int n)`，将n个字节读入buf，返回读到的字节数/0(EOF)

  write函数原型`write(int fd, char *buf, int n)`，将n个字节从buf写入fd中，返回n

## Primes(hard)

**具体实现**：

参照下列伪代码：

```pseudocode
p = get a number from left neighbor
print p
loop:
    n = get a number from left neighbor
    if (p does not divide n)
        send n to right neighbor
```

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int MIN_NUM = 2;
const int MAX_NUM = 35;

// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor

void
pipelined_prime(int p[2])
{
	int prime;
	int n;
	int pid;
	close(p[1]);
	if(read(p[0], &prime, 4) != 4){
		fprintf(2, "child proc failed to read\n");
		exit(1);
	}
	printf("prime %d\n", prime);

	if(read(p[0], &n, 4)){
		int p_new[2];
		pipe(p_new);
		pid = fork();
		if (pid < 0){
			fprintf(2, "new proc failed to create\n");
			exit(1);
		}else if (pid == 0) pipelined_prime(p_new);
		else{
			close(p_new[0]);
			if(n % prime) write(p_new[1], &n, 4);
			
			while(read(p[0], &n, 4)){
				if(n % prime)write(p_new[1], &n, 4);
			}
			close(p[0]);
			close(p_new[1]);
			wait(0);
		}
	}
	exit(0);
}

int 
main(int argc, char const *argv[])
{
	int p[2];
	int pid;
	pipe(p);
	pid = fork();
	if (pid < 0){
		fprintf(2, "child proc failed to create\n");
		exit(1);
	} else if (pid == 0) pipelined_prime(p);
	else{
		close(p[0]);
		for(int i=MIN_NUM; i<=MAX_NUM; i++){
			if (write(p[1], &i, 4) != 4){
				fprintf(2, "first process failed to write %d into the pipe\n", i);
				exit(1);
			}
		}
		close(p[1]);
		wait(0);
		exit(0);
	}
	return 0;
}
```

**Note**:

- 在pipelined_prime中，循环读入n处先要创建管道，否则无法正常读入数字

## Find(moderate)

编写一个简单版本的 UNIX 查找程序：查找目录树中具有特定名称的所有文件。

**具体实现**：根据hint参考`user/ls.c`学习如何遍历文件目录，使用递归进行查找

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void 
find(const char *path, const char *filename)
{
    char buf[512], *p;
    
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (read(fd, &de, sizeof(de)) != sizeof(de)){
        exit(1);
    }

    switch(st.type){
        case T_FILE:
            // if name of the current file is {filename}, then print result and return 
            if (strcmp(de.name, filename) == 0){
                printf("%s/%s\n", path, filename);
            }
            break;

        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                fprintf(2, "find: path too long\n");
                break;
            }
            // get the next level of path
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
				// According to the given hint: Don't recurse into "." and "..".
                if((de.inum == 0) || (strcmp(de.name, ".") == 0) || (strcmp(de.name, "..") == 0)){
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                // 获得文件描述
                if(stat(buf, &st) < 0){
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }

                if (st.type == T_FILE){
                    if (strcmp(de.name, filename) == 0){
                        printf("%s\n", buf);
                    }
                }else if (st.type == T_DIR){
                    // Recursively find the file
                    find(buf, filename);
                }
            }
            break;
        }
    close(fd);
}

int 
main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }

    char *path = argv[1];
    char *filename = argv[2];
    find(path, filename);

    exit(0);
}
```

## Xargs(moderate)

编写一个简单版本的 UNIX xargs 程序：从标准输入读取行并为每一行运行一个命令，将该行作为参数提供给命令

**具体实现**：主要对输出进行读入以及处理(C没有`getline`和`split`实在是太麻烦啦)

```C
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
    // get from "kernel/param.h"
    char *args[MAXARG];
    char buf[512];
    int n;
    int pid;

    for(int i=1; i<argc; i++) args[i-1] = argv[i];
    
    do{ 
        char *p = buf;
        //Read until meet '\n'
        n = read(0, p, 1);
        while(n) {
            if (*p == '\n' || *p == '\0'){
                *p = '\0';
                break;
            }
            p++;
            n = read(0, p, 1);
        } 

        if(p != buf){
            pid = fork();
            if (pid < 0) {
                fprintf(2, "child proc failed to create\n");
                exit(1);
            }else if(pid == 0){
                args[argc-1] = buf;
                args[argc] = '\0';
                exec(argv[1], args);
                fprintf(2, "exec failed\n");
                exit(1);
            }else wait(0);
        }
    }while(n > 0);
    exit(0);
}
```

## 总结与心得

All testcases passed：

<img src="/Users/xmtx/Library/Application Support/typora-user-images/截屏2022-08-16 21.11.06.png" alt="截屏2022-08-16 21.11.06" style="zoom:50%;" />

这个lab整体难度不大，重点在于熟悉xv6的使用以及了解各种syscall的使用