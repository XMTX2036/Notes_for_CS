# LEC5 GDB, calling conventions and stack frames

## Calling Conventions

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-05%2017.49.03.png" alt="截屏2022-09-05 17.49.03" style="zoom:50%;" />

**传入参数**

- 小于一个指针大小(`RV64`:8 bytes, `RV32`: 4 bytes)的参数传入时将参数放在寄存器的最低位，当2个指针字的参数传入时，低位的1个指针字放在偶数寄存器，比如a0上，高位的1个指针字放在奇数寄存器，比如a1上

  arg1 ===> a0

  arg2 ===> a1

- 当大于2个指针大小的参数传入时以**引用**的方式传入，`struct`参数没有传到寄存器的部分将以**栈**的方式传入，`sp`栈指针将指向第一个没有传入到寄存器的参数

**返回值**

- 如果是整数将放在a0和a1中，如果是小数将放置在fa0和fa1寄存器中
- 对于更大的返回值，将放置在内存中，caller开辟这个内存，并且把指向这个内存的指针作为第一个参数传递给callee

## Stack

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-05%2020.43.54.png" alt="截屏2022-09-05 20.43.54" style="zoom:50%;" />