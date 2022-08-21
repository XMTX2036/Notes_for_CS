# CSAPP

## Lec1 Course Overview

### Great Reality #1

Ints are not Integers, Floats are not Reals

e.g.1  Is $x^2\gt0$?

For Floats ===> Yes

For Ints ===> No

e.g.2 Is $(x+y)+z = x+(y+z)$?

For unsigned & signed ints ===> Yes

For floats ===> No

### Computer Arithmetic

- Don't generate random values
- Cannot assume all "usual" mathiematical properties
- Observation

### Great Reality #2 You've Got to Know Assembly

**machine-level execution model**

- Behavior of programs in presence of bugs
- Tuning program performance
- Implementing system software
- Creating/fighting malware ===> x86 assembly

### Great Reality #3 Memory Matters

#### Random Access Memory Is an Unphysical Abstraction

- Memory is not unbounded
- Memory referencing bugs especially pernicious
- Memory performance is not uniform

#### Memory Referencing Errors

- C and C++ do not provide any memory protection
  - Out of bounds array references
  - Invalid pointer values
  - Abuses of malloc/free
- Can lead to nasty bugs
- How can I deal with this?
  - Program in Java, Ruby, Python, ML, ...
  - Understand what possible interactions may occur
  - Use or develop tools to detect referencing errors(e.g. Valgrind)

### Great Reality #4 There's more to performance than asymptotic complexity

- Constant factors matter too!
- And even exact op count does not predict performance
- Must understand system to optimize performance

### Great Reality #5 Computers do more than execute programs



### Course Perspective

- Computer Architecture
- Operating Systems
- Compilers
- Networking



### Programs and Data

- Topics 
  - Bits operations, arithmetic, assembly language programs
  - Representation of C control and data structures
  - Includes aspects of architecture and compilers
- Assignments 
  - L1(datalab): Manipulating bits
  - L2(bomblab): Defusing a binary bomb
  - L3(attacklab): The basics of code injection attacks

### The Memory Hierarchy

- Topics
  - Memory technology, memory hierarchy, caches, disks, locality
  - Includes aspects of architecture and OS
- Assignments
  - L4(cachelab): Building a cache simulator and optimizing for locality
    - Learn how to exploit locality in your programs

### Exceptional Control Flow

- Topics
  - Hardware exceptions, processes, process control, Unix signals, nonlocal jumps
  - Includes aspects of compilers, OS, and architecture
- Assignments
  - L5(tshlab): Writing your own Unix shell
    - A first introduction to concurrency



### Virtual Memory

- Topics
  - Virtual memory, address translation, dynamic storage allocation
  - Includes aspects architecture and OS
- Assignments
  - L6(malloclab): Writing your own malloc package
    - Get a real feel for systems-level programming



### Networking and Concurrency

- Topcis
  - High level and low level I/O, network programming
  - Internet services, Web servers
  - concurrency, concurrent server design, threads
  - I/O multiplexing with select
  - Includes aspects of networking, OS and architecture
- Assignments
  - L7(proxylab): Writing your own Web proxy
    - Learn network programming and more about concurrency and synchronization



## Lec2 Bits, Bytes and Integers

### Bits & Bytes

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-08-21%2017.37.48.png" alt="截屏2022-08-21 17.37.48" style="zoom:50%;" />

### **Unsigned & Signed Values**

Unsigned: $B2U = \Sigma_{i=0}^{w-1}x_i2^i$

Signed: $B2T=-x_{w-1}2^{w-1}+\Sigma_{i=0}^{w-2}x_i2^i$

### Casting Surprises

- If there is a mix of unsigned and signed in single expression, **signed values implicitly cast to unsigned**

  - e.g.

    ```C
    -1 > 0U;
    2147483647U < -2147483647-1;
    ```

- Casting signed <===> unsigned: adding/subtracing $2^w$

### Expanding & Truncating

===> Truncating: 

- Unsigned:

  11011 = 1+2+8+16 = 27

  Drop the highest: 1011 = 1+2+8 = 11(do module operation)

- Signed:

  1011 = -13

  011 = 3 

### Why Should I use Unsigned?

**Don't use without understanding implications**

```C
// Easy to make mistakes
unsigned i;
for(i=cnt-2; i>=0; i--){
  a[i] += a[i+1];
}

// Can be very subtle
#define DELTA sizeof(int)
int i;
for(i=CNT; i-DELTA>=0; i-=DELTA){
  // ...
}

// Proper way to use unsigned as loop index
unsigned i;
for(i=cnt-2; i<cnt; i--){
  a[i] += a[i+1];
}
// Even better
size_t i;
for(i=cnt-2; i<cnt; i--){
  a[i] += a[i+1];
}
```

### Representations in memory, pointers, strings

