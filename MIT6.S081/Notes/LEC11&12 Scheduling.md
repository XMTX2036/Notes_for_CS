# LEC11&12 Scheduling

**Why support multiple tasks?**

- **time-sharing**: many users and/or many running programs
- **program structure**: prime number sieve
- **parallel speedup on multi-core hardware**.

*thread = an independent serial execution -- registers, pc, stack*

**two main strategies:**

- multiple CPUs, each CPU runs a different thread
- each CPU "switches" between threads, runs one at a time

**Thread can share memory or not**

- xv6 kernel threads: they share kernel memory (thus locks)
- xv6 user processes: one thread per process, so no sharing
- Linux: supports multiple threads sharing a user process's memory

**Other techniques for interleaving multiple tasks**

- look up event-driven programming
- state machines

*threads are usually the most convenient*

**线程设计**

- 如何在多CPU行交错执行多个线程
  - 如何透明地交错执行
  - 调度(Scheduling) ===> 选择接下来执行线程
- 在线程不执行时需要保存什么
  - registers, stack, memory
  - 跟踪每个线程的状态
    - RUNNING
    - RUNNABLE
    - SLEEPING
- 如何处理计算绑定线程(compute-bound threads)