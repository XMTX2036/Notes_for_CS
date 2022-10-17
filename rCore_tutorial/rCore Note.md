# rCore Note

## Chapter0 操作系统概述

### 什么是操作系统

OS两个功能：

- 向下管理并控制计算机硬件和各种外设
- 向上管理应用软件并提供各种服务

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2016.41.33.png" alt="截屏2022-10-17 16.41.33" style="zoom:50%;" />

执行环境(Execution Environment)：

- 硬件
- 操作系统内核
- 运行时库
- 图形界面支持库等

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2016.45.52.png" alt="截屏2022-10-17 16.45.52" style="zoom:50%;" />

### OS的系统调用接口

**API与ABI**

**系统调用接口与功能**

- 一个运行的程序如何能输出字符信息？如何能获得输入字符信息？
- 一个运行的程序可以要求更多（或更少）的内存空间吗？
- 一个运行的程序如何持久地存储用户数据？
- 一个运行的程序如何与连接到计算机的设备通信并通过它们与物理世界通信？
- 多个运行的程序如何同步互斥地对共享资源进行访问？
- 一个运行的程序可以创建另一个程序的实例吗？需要等待另外一个程序执行完成吗？一个运行的程序能暂停或恢复另一个正在运行的程序吗？

操作系统主要通过**基于ABI 的系统调用接口**来给应用程序提供上述服务，以支持应用程序的各种需求

- 进程（即程序运行过程）管理：复制创建进程 fork 、退出进程 exit 、执行进程 exec 等
- 线程管理：线程（即程序的一个执行流）的创建、执行、调度切换等
- 线程同步互斥的并发控制：互斥锁 mutex 、信号量 semaphore 、管程 monitor 、条件变量 condition variable 等
- 进程间通信：管道 pipe 、信号 signal 、事件 event 等
- 虚存管理：内存空间映射 mmap 、改变数据段地址空间大小 sbrk 、共享内存 shm 等
- 文件 I/O 操作：对存储设备中的文件进行读 read 、写 write 、打开 open 、关闭 close 等操作
- 外设 I/O 操作：外设包括键盘、显示器、串口、磁盘、时钟 … ，主要采用文件 I/O 操作接口

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2016.55.03.png" alt="截屏2022-10-17 16.55.03" style="zoom:50%;" />

### 操作系统抽象

**执行环境**

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2017.01.20.png" alt="截屏2022-10-17 17.01.20" style="zoom:50%;" />

对于应用程序的执行环境而言，应用程序只能看到执行环境直接提供给它的接口（API 或 ABI），这使得应用程序所能得到的服务取决于执行环境提供给它的访问接口

`定义`

**执行环境**是应用程序正确运行所需的服务与管理环境，用来完成应用程序在运行时的数据与资源管理、应用程序的生存期等方面的处理，它定义了应用程序有权访问的其他数据或资源，并决定了应用程序的行为限制范围

**普通控制流(CCF)**

应用程序仅能接触到其当前所在的执行环境下，不会跳到其他环境

**异常控制流(ECF)**

一种控制流的“突变”，即控制流脱离了其所在的执行环境，并产生执行环境的切换

应用程序 *感知* 不到这种异常的控制流情况，这主要是由于操作系统把这种情况 *透明* 地进行了执行环境的切换和对各种异常情况的处理，让应用程序从始至终地 *认为* 没有这些异常控制流的产生

**控制流上下文(Context)**

控制流在执行完某指令时的物理资源内容，即确保下一时刻能继续 *正确* 执行控制流指令的物理资源内容称为控制流的**上下文** (Context) ，也可称为控制流所在执行环境的状态

- 物理资源：即计算机硬件资源，如CPU的寄存器、可访问的物理内存等
- 虚拟资源：即操作系统提供的资源，如文件，网络端口号，网络地址，信号等

上下文处理：

- 异常控制流的上下文： CPU 和操作系统协同完成
- 函数转移控制流的上下文保存与恢复：编译器

三类异常控制流

- 外设中断 (Device Interrupt) 

  异步产生，与处理器执行无关

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2017.20.14.png" alt="截屏2022-10-17 17.20.14" style="zoom:50%;" />

- 陷入 (Trap) 

  要通过syscall请求操作系统服务而**有意引发**的事件

- 异常 (Exception，也称Fault Interrupt)

  在处理器执行指令期间检测到**不正常的或非法的内部事件**

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2017.21.59.png" alt="截屏2022-10-17 17.21.59" style="zoom:50%;" />

**进程(Process)**

一个进程是一个具有一定独立功能的程序在一个数据集合上的一次动态执行过程

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2019.38.43.png" alt="截屏2022-10-17 19.38.43" style="zoom:50%;" />

进程上下文切换

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2019.39.47.png" alt="截屏2022-10-17 19.39.47" style="zoom:50%;" />

**地址空间**

虚拟(地址)空间Virtual Memory ===> 对物理地址Physical Address/Memory的虚拟化和抽象

- 大 ===> 可能超过计算机的物理内存容量
- 连续的地址空间编址
- 私有的 ===> 别的应用程序无法破坏

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2019.47.46.png" alt="截屏2022-10-17 19.47.46" style="zoom:50%;" />

**文件**

主要用于对持久存储的抽象，并进一步扩展到为外设的抽象

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2019.48.47.png" alt="截屏2022-10-17 19.48.47" style="zoom:50%;" />

### 操作系统的特征

- 虚拟化 (Virtualization)

  - 内存虚拟化

  - CPU虚拟化

    每个程序都感觉自己是独占CPU的

- 并发性 (Concurrency)

- 异步性

  调度 & 中断

  应用程序执行完成时间不可预测

- 共享性

- 持久性 (Persistency)

## Chapter1 应用程序与基本执行环境

### 应用程序执行环境与平台支持

**应用程序执行环境**

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2020.30.17.png" alt="截屏2022-10-17 20.30.17" style="zoom:50%;" />

> All problems in computer science can be solved by another level of indirection.
>
> -- David Wheeler

硬件分类：

- CPU
- Memory
- I/O device

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2020.26.26.png" alt="截屏2022-10-17 20.26.26" style="zoom:50%;" />

```bash
write(1, "Hello, world!\n", 14Hello, world!
)         = 14
sigaltstack({ss_sp=NULL, ss_flags=SS_DISABLE, ss_size=8192}, NULL) = 0
munmap(0x7fb9b2850000, 12288)           = 0
exit_group(0)                           = ?
+++ exited with 0 +++
```

**平台与目标三元组**

平台主要是指 CPU 类型、操作系统类型和标准运行时库的组合

目标三元组主要是CPU 架构、CPU 厂商、操作系统和运行时库

- 如果用户态基于的内核不同，会导致系统调用接口不同或者语义不一致
- 如果底层硬件不同，对于硬件资源的访问方式会有差异；特别是如果 ISA 不同，则向软件提供的指令集和寄存器都不同

### 移除标准库依赖

```rust
// os/src/main.rs
#![no_main]
#![no_std]
mod lang_items;
// ... other code


// os/src/lang_items.rs
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
```

### 内核第一条指令

**QEMU启动流程**

- 第一阶段：将必要的文件载入到 Qemu 物理内存之后，Qemu CPU 的程序计数器PC会被初始化为 `0x1000` ，因此 Qemu 实际执行的第一条指令位于物理地址 `0x1000` ，接下来它将执行寥寥数条指令并跳转到物理地址 `0x80000000` 对应的指令处并进入第二阶段。从后面的调试过程可以看出，该地址 `0x80000000` 被固化在 Qemu 中，作为 Qemu 的使用者，我们在不触及 Qemu 源代码的情况下无法进行更改
- 第二阶段：由于 Qemu 的第一阶段固定跳转到 `0x80000000` ，我们需要将负责第二阶段的 bootloader `rustsbi-qemu.bin` 放在以物理地址 `0x80000000` 开头的物理内存中，这样就能保证 `0x80000000` 处正好保存 bootloader 的第一条指令。在这一阶段，bootloader 负责对计算机进行一些初始化工作，并跳转到下一阶段软件的入口，在 Qemu 上即可实现将计算机控制权移交给我们的内核镜像 `os.bin` 。这里需要注意的是，对于不同的 bootloader 而言，下一阶段软件的入口不一定相同，而且获取这一信息的方式和时间点也不同：入口地址可能是一个预先约定好的固定的值，也有可能是在 bootloader 运行期间才动态获取到的值。我们选用的 RustSBI 则是将下一阶段的入口地址预先约定为固定的 `0x80200000` ，在 RustSBI 的初始化工作完成之后，它会跳转到该地址并将计算机控制权移交给下一阶段的软件——也即我们的内核镜像
- 第三阶段：为了正确地和上一阶段的 RustSBI 对接，我们需要保证内核的第一条指令位于物理地址 `0x80200000` 处。为此，我们需要将内核镜像预先加载到 Qemu 物理内存以地址 `0x80200000` 开头的区域上。一旦 CPU 开始执行内核的第一条指令，证明计算机的控制权已经被移交给我们的内核，也就达到了本节的目标

**程序内存布局与编译流程**

Memory Layout ===> section

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2022.34.03.png" alt="截屏2022-10-17 22.34.03" style="zoom:50%;" />

**编译流程**

- **编译器** (Compiler) 将每个源文件从某门高级编程语言转化为汇编语言，注意此时源文件仍然是一个 ASCII 或其他编码的文本文件；

- **汇编器** (Assembler) 将上一步的每个源文件中的文本格式的指令转化为机器码，得到一个二进制的**目标文件** (Object File)；

- **链接器** (Linker) 将上一步得到的所有目标文件以及一些可能的外部目标文件链接在一起形成一个完整的可执行文件

  - 将来自不同目标文件的段在目标内存布局中重新排布

    <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-10-17%2022.40.50.png" alt="截屏2022-10-17 22.40.50" style="zoom:50%;" />

  - 将符号替换为具体地址