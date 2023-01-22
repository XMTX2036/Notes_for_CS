# CSAPP Book Note

> @Ref: 
>
> - CSAPP Book
> - [b站九曲阑干 CSAPP解读](https://www.bilibili.com/video/BV1cD4y1D7uR/?vd_source=f21668f4bb00ffca64ecc38470067c5e)

## Summary

- Chapter1 A Tour of Computer Systems
- **Part1** Program Structure and Execution
- **Part2** Running Programs on a System
- **Part3** Interaction and Communication between Programs

## Chapter1 A Tour of Computer Systems

> The Lifetime of Hello Program
>
> - **Step1** Create
> - **Step2** Compile
> - **Step3** Run
> - **Step4** Exit

### 1.1 Information Is Bits + Context

### 1.2 Programs Are Translated by Other Programs into Different Forms

```c
// hello.c
// compile the source program: gcc -o hello hello.c
#include <stdio.h>

int main(int argc, char *argv[]) 
{
  printf("Hello World!\n");
  return 0;
}
```



<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2010.34.38.png" alt="截屏2023-01-22 10.34.38" style="zoom:50%;" />

#### The Compilation System

- Preprocessor
- Compiler
- Assembler
- Linker

### 1.3 It Pays to Understand How Compilation Systems Work

**Reasons**

- Optimizing program performance
- Understanding link-time errors
- Avoiding security holes

### 1.4 Processors Read and Interpret Instructions Stored in Memory

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2010.45.45.png" alt="截屏2023-01-22 10.45.45" style="zoom:50%;" />

### 1.5 Caches Matter

Process-Memory gap

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2010.48.20.png" alt="截屏2023-01-22 10.48.20" style="zoom:50%;" />

### 1.6 Storage Devices Form a Hierarchy

### 1.7 The Operating System Manages the Hardware

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2010.56.50.png" alt="截屏2023-01-22 10.56.50" style="zoom:50%;" />

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2011.25.35.png" alt="截屏2023-01-22 11.25.35" style="zoom:50%;" />

### 1.8 Systems Communicate with Other Systems Using Networks

### 1.9 Important Themes

#### 1.9.1 Amdahl's Law

$$
S = \frac{T_{old}}{T_{new}} = \frac{1}{(1-\alpha) + \frac{\alpha}{k}}
$$

$\alpha$表示程序可优化的部分，$k$为加速因子

#### 1.9.2 Concurrency and Parallelism

- Thread-Level Concurrency

- Instruction-Level Parallelism，ILP

- Single-Instruction Multiple-Data Parallelism，SIMD

  <img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2011.33.32.png" alt="截屏2023-01-22 11.33.32" style="zoom:50%;" />

## Chapter2 Representing and Manipulating Information

### Outline

- Information Storage
- Integer Representations
- Integer Arithmetic
- Floating Point

### 2.1 Information Storage

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2011.39.31.png" alt="截屏2023-01-22 11.39.31" style="zoom:50%;" />

**Words**

Word Size = $w$ bit(s)

Virtual Address Space = 0~$2^w-1$

32bit ===> 4GB

```bash
# 32-bit program
gcc -m32 -o hello32 hello.c
# 64-bit program
gcc -m64 -o hello64 hello.c
```

**Address and Byte Ordering**

- Big endian

  高位 ===> 高地址

- Little endian

  低位 ===> 低地址

**Shift Operation in C**

- Arithmetic Right shift

  有符号数右移操作时，默认使用算数右移

- Logical Right Shift

  直接补0

### 2.2 Integer Representations

**SIgned versus Unsigned in C**

一个运算数是有符号数，另一个数是无符号数 ===> C会隐式地有符号数强制转换为无符号数

### 2.3 Integer Arithmetic

**Division**

Two's-complement Division by a Power of 2

```c
(x < 0 ? x+(1 << k)-1 : x) >> k; // 加入bias来控制舍入的正确性
```

### 2.4 Floating Point

$$
V = (-1)^s\times M\times 2^E
$$

#### Floating-Point Type

> For single precision ===> float in C

- Normalized Values

  exp $\neq 0$ and $\neq 255$ 

  $e_{min} = 1$, $e_{max} = 254$

  $E = e - bias$

  > $bias = 2^{sizeof(exp)}-1$

  $bias_{float} = 127$

  $bias_{double} = 1023$

  $M = 1+f$

- Denormalized Values 

  exp $=$ 0

  - Case1 $s = 0$ $M = f = 0$

    `+0.0`

  - Case2 $s = 1$ $M = f = 0$

    `-0.0`

- Special Values

  exp为全1

  - Infinity

    - $+\infin$ 

      $s = 0$, $f = 0$

    - $-\infin$

      $s = 1$, $f = 0$ 

  - NaN

    $f\neq 0$

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2023-01-22%2015.31.59.png" alt="截屏2023-01-22 15.31.59" style="zoom:50%;" />

#### Rounding

- Round-to-**even**

  1.4 ===> 1

  1.6 ===> 2

  1.5 ===> ? `2`

  2.5 ===> ? `2`

- Round-toward-zero

- Round-down

- Round-up