# CSAPP bomblab

## Approach1 IDA

将可执行文件用IDA打开，F5得到反汇编的伪代码

整体代码如下：

```C
// local variable allocation has failed, the output may be wrong!
int __cdecl main(int argc, const char **argv, const char **envp)
{
  const char **v3; // rbx
  __int64 line; // rax
  __int64 v5; // rax
  __int64 v6; // rax
  __int64 v7; // rax
  __int64 v8; // rax
  __int64 v9; // rax

  if ( argc == 1 )
  {
    infile = (FILE *)stdin;
  }
  else
  {
    v3 = argv;
    if ( argc != 2 )
    {
      __printf_chk(1LL, "Usage: %s [<input_file>]\n", *argv);
      exit(8);
    }
    *(_QWORD *)&argc = argv[1];
    argv = (const char **)"r";
    infile = fopen(*(const char **)&argc, "r");
    if ( !infile )
    {
      __printf_chk(1LL, "%s: Error: Couldn't open %s\n", *v3, v3[1]);
      exit(8);
    }
  }
  initialize_bomb(*(_QWORD *)&argc, argv, envp);
  puts("Welcome to my fiendish little bomb. You have 6 phases with");
  puts("which to blow yourself up. Have a nice day!");
  line = read_line();
  phase_1(line);
  phase_defused();
  puts("Phase 1 defused. How about the next one?");
  v5 = read_line();
  phase_2(v5);
  phase_defused();
  puts("That's number 2.  Keep going!");
  v6 = read_line();
  phase_3(v6);
  phase_defused();
  puts("Halfway there!");
  v7 = read_line();
  phase_4(v7);
  phase_defused();
  puts("So you got that one.  Try this one.");
  v8 = read_line();
  phase_5(v8);
  phase_defused();
  puts("Good work!  On to the next...");
  v9 = read_line();
  phase_6(v9);
  phase_defused();
  return 0;
}
```

### Phase1

其中`phase_1`函数如下：

```C
__int64 __fastcall phase_1(__int64 a1)
{
  __int64 result; // rax

  result = strings_not_equal(a1, "Border relations with Canada have never been better.");
  if ( (_DWORD)result )
    explode_bomb();
  return result;
}
```

很轻松可以得到，第一次输入应为`Border relations with Canada have never been better.`

### Phase2

```C
__int64 __fastcall phase_2(__int64 a1)
{
  __int64 result; // rax
  char *v2; // rbx
  int v3; // [rsp+0h] [rbp-38h] BYREF
  char v4; // [rsp+4h] [rbp-34h] BYREF
  char v5; // [rsp+18h] [rbp-20h] BYREF
  // 1 
  read_six_numbers(a1, &v3);
  // 如果第一个数字不是1就爆炸，
  if ( v3 != 1 )
    explode_bomb();
  // v2[0]表示第一个读入的数字
  v2 = &v4;
  do
  {
    // 读入的六个数字a1～a6， ai*2 = a（i+1），故成等比数列
    result = (unsigned int)(2 * *((_DWORD *)v2 - 1));
    if ( *(_DWORD *)v2 != (_DWORD)result )
      explode_bomb();
    v2 += 4;
  }
  while ( v2 != &v5 );
  return result;
}
```

随后进一步查看`read_six_numbers`函数

```C
__int64 __fastcall read_six_numbers(__int64 a1, __int64 a2)
{
  __int64 result; // rax
	// &unk_4025C3 表示%d %d %d %d的格式化读取
  // a1 为输入的input字符串
  result = __isoc99_sscanf(a1, &unk_4025C3, a2, a2 + 4, a2 + 8, a2 + 12, a2 + 16, a2 + 20);
  // 必须读取6个以上的数字
  if ( (int)result <= 5 )
    explode_bomb();
  return result;
}
```

由上述代码分析可知`phase2	`为`1 2 4 8 16 32`

### Phase3

```C
__int64 __fastcall phase_3(__int64 a1)
{
  __int64 result; // rax
  int v2; // [rsp+8h] [rbp-10h] BYREF
  int v3; // [rsp+Ch] [rbp-Ch] BYREF

  if ( (int)__isoc99_sscanf(a1, "%d %d", &v2, &v3) <= 1 )
    explode_bomb();
  switch ( v2 )
  {
    case 0:
      result = 207LL;
      break;
    case 1:
      result = 311LL;
      break;
    case 2:
      result = 707LL;
      break;
    case 3:
      result = 256LL;
      break;
    case 4:
      result = 389LL;
      break;
    case 5:
      result = 206LL;
      break;
    case 6:
      result = 682LL;
      break;
    case 7:
      result = 327LL;
      break;
    default:
      explode_bomb();
  }
  if ( (_DWORD)result != v3 )
    explode_bomb();
  return result;
}
```

根据分析Phase3存在8对可行解`0 207`,`1 311`, `2 707`,`3 256`, `4 389`, `5 206`, `6 682`, `7 327`

### Phase4

```C
__int64 __fastcall phase_4(__int64 a1)
{
  __int64 result; // rax
  unsigned int v2; // [rsp+8h] [rbp-10h] BYREF
  int v3; // [rsp+Ch] [rbp-Ch] BYREF

  if ( (unsigned int)__isoc99_sscanf(a1, "%d %d", &v2, &v3) != 2 || v2 > 0xE )
    explode_bomb();
  result = func4(v2, 0LL, 14LL);
  // v3必须为0
  if ( (_DWORD)result || v3 )
    explode_bomb();
  return result;
}
```

其中func4具体如下：

```C
// 当调用该函数时，a2 = 0, a3 = 14
__int64 __fastcall func4(__int64 a1, __int64 a2, __int64 a3)
{
  int v3; // ecx
  __int64 result; // rax
	// v3 = 7;
  v3 = ((int)a3 - (int)a2) / 2 + a2;
  if ( v3 > (int)a1 )
    return 2 * (unsigned int)func4(a1, a2, (unsigned int)(v3 - 1));
  result = 0LL;
  // 不能进入下面的分支，同时也不能在之前提前返回，a1只可取7
  if ( v3 < (int)a1 )
    return 2 * (unsigned int)func4(a1, (unsigned int)(v3 + 1), a3) + 1;
  return result;
}
```

故phase4为`7 0`

### Phase5

```C
unsigned __int64 __fastcall phase_5(__int64 a1)
{
  __int64 i; // rax
  char v3[8]; // [rsp+10h] [rbp-18h] BYREF
  unsigned __int64 v4; // [rsp+18h] [rbp-10h]

  v4 = __readfsqword(0x28u);
  if ( (unsigned int)string_length() != 6 )
    explode_bomb();
  for ( i = 0LL; i != 6; ++i )
    v3[i] = array_3449[*(_BYTE *)(a1 + i) & 0xF];
  v3[6] = 0;
  // 9 15 14 5 6 7 ===> IiYy Oo Nn EeUu FfVv GgWw
  if ( (unsigned int)strings_not_equal(v3, "flyers") )
    explode_bomb();
  return __readfsqword(0x28u) ^ v4;
}
```

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2022.52.24.png" alt="截屏2022-08-21 22.52.24" style="zoom:50%;" />

由上述Python代码可知对应A-Za-z的字母&0xF后的结果，只要选取对应index序列`9 15 14 5 6 7`即为满足要求的phase5

### Phase6

```C
__int64 __fastcall phase_6(__int64 a1)
{
  int *v1; // r13
  int v2; // r12d
  int v3; // ebx
  char *v4; // rax
  unsigned __int64 i; // rsi
  _QWORD *v6; // rdx
  int v7; // eax
  int v8; // ecx
  __int64 v9; // rbx
  char *v10; // rax
  __int64 j; // rcx
  __int64 v12; // rdx
  int v13; // ebp
  __int64 result; // rax
  int v15[6]; // [rsp+0h] [rbp-78h] BYREF
  char v16; // [rsp+18h] [rbp-60h] BYREF
  __int64 v17; // [rsp+20h] [rbp-58h]
  char v18; // [rsp+28h] [rbp-50h] BYREF
  char v19[40]; // [rsp+50h] [rbp-28h] BYREF

  v1 = v15;
  read_six_numbers(a1, (__int64)v15);
  v2 = 0;
  while ( 1 )
  {
    if ( (unsigned int)(*v1 - 1) > 5 )
      explode_bomb(a1, v15);
    if ( ++v2 == 6 )
      break;
    v3 = v2;
    do
    {
      if ( *v1 == v15[v3] )
        explode_bomb(a1, v15);
      ++v3;
    }
    while ( v3 <= 5 );
    ++v1;
  }
  // 上述循环要求：6个输入的数字均小于等于6且互不相等
  v4 = (char *)v15;
  do
  {
    *(_DWORD *)v4 = 7 - *(_DWORD *)v4;
    v4 += 4;
  }
  // 6个数分别变为7-自身===> a[i] = 7-a[i]
  while ( v4 != &v16 );
  for ( i = 0LL; i != 24; i += 4LL )
  {
    v8 = v15[i / 4];
    if ( v8 <= 1 )
    {
      v6 = &node1;
    }
    else
    {
      v7 = 1;
      v6 = &node1;
      do
      {
        v6 = (_QWORD *)v6[1];
        ++v7;
      }
      while ( v7 != v8 );
    }
    *(__int64 *)((char *)&v17 + 2 * i) = (__int64)v6;
  }
  v9 = v17;
  v10 = &v18;
  for ( j = v17; ; j = v12 )
  {
    v12 = *(_QWORD *)v10;
    *(_QWORD *)(j + 8) = *(_QWORD *)v10;
    v10 += 8;
    if ( v10 == v19 )
      break;
  }
  *(_QWORD *)(v12 + 8) = 0LL;
  v13 = 5;
  do
  {
    result = **(unsigned int **)(v9 + 8);
    if ( *(_DWORD *)v9 < (int)result )
      explode_bomb(a1, v19);
    v9 = *(_QWORD *)(v9 + 8);
    --v13;
  }
  while ( v13 );
  return result;
}
```

经过一统狂绕之后可以得到结果：`4 3 2 1 6 5`

### Secret Phase

纵观整个完成过程，我们可以发现有一个函数的作用&其内部功能从来未涉及===>`phase_defused`

```C
unsigned __int64 phase_defused()
{
  char v1; // [rsp+8h] [rbp-70h] BYREF
  char v2; // [rsp+Ch] [rbp-6Ch] BYREF
  char v3[88]; // [rsp+10h] [rbp-68h] BYREF
  unsigned __int64 v4; // [rsp+68h] [rbp-10h]

  v4 = __readfsqword(0x28u);
  if ( num_input_strings == 6 )
  {
    // 在满足此处输入的phase后增加一条DrEvil
    if ( (unsigned int)__isoc99_sscanf(&unk_603870, "%d %d %s", &v1, &v2, v3) == 3
      && !(unsigned int)strings_not_equal(v3, "DrEvil") )
    {
      puts("Curses, you've found the secret phase!");
      puts("But finding it and solving it are quite different...");
      secret_phase();
    }
    puts("Congratulations! You've defused the bomb!");
  }
  return __readfsqword(0x28u) ^ v4;
}
```

其中secret_phase函数的具体实现如下：

```C
unsigned __int64 secret_phase()
{
  const char *line; // rdi
  unsigned int v1; // ebx

  line = (const char *)read_line();
  v1 = strtol(line, 0LL, 10);
  // v1 <= 1001
  if ( v1 - 1 > 0x3E8 )
    explode_bomb();
  if ( (unsigned int)fun7(&n1, v1) != 2 )
    explode_bomb();
  puts("Wow! You've defused the secret stage!");
  return phase_defused();
}
```

其中fun7的具体实现如下：

```C
__int64 __fastcall fun7(__int64 a1, __int64 a2)
{
  __int64 result; // rax

  if ( !a1 )
    return 0xFFFFFFFFLL;
  if ( *(_DWORD *)a1 > (int)a2 )
    return 2 * (unsigned int)fun7(*(_QWORD *)(a1 + 8), a2);
  result = 0LL;
  if ( *(_DWORD *)a1 != (_DWORD)a2 )
    return 2 * (unsigned int)fun7(*(_QWORD *)(a1 + 16), a2) + 1;
  // return value must be 2！
  return result;
}
```

发现是与之前Phase4类似的递归，倒推即可

secret phase为 `22`

### 整合exp.txt

```bash
Border relations with Canada have never been better.
1 2 4 8 16 32
2 707
7 0 DrEvil
yonuvw
4 3 2 1 6 5 
22
```

## Approach2 GDB

### Phase1

在phase_1函数处下断点

```python
start
b phase_1
c
si
```

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2023.31.26.png" alt="截屏2022-08-21 23.31.26" style="zoom:50%;" />

​		可以发现phase_1函数内部调用了一个`strings_not_equal`函数，根据gdb显示此函数有两个参数分别存在\$rdi与\$rsi寄存器中，其中前者存入笔者输入的Hello!Test!，则另一个参数应当为需要进行对比的判据，结果也与方法一获得的一致；

​		输入`r`重新运行gdb，重复上述操作，将输入改为获得的正确内容，继续观察程序行为

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2023.44.28.png" alt="截屏2022-08-21 23.44.28" style="zoom:50%;" />

直接`n`将该函数运行完成，进入下一个函数

### Phase2

```python
b phase_2
c
```

随后可以看到如下内容：

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2023.46.52.png" alt="截屏2022-08-21 23.46.52" style="zoom:50%;" />

可见在phase_2内部会调用一个名为read_six_numbers的函数，故可先得知该轮输入的要求为六个数字

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2023.49.51.png" alt="截屏2022-08-21 23.49.51" style="zoom:50%;" />

​		由上图可见，六个数字已被读入stack中