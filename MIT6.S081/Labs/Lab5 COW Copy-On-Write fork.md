# Lab5 COW: Copy-On-Write fork

> There is a saying in computer systems that any systems problem can be solved with a level of indirection. 

xv6 中的`fork()`系统调用将所有父进程的用户空间内存复制到子进程中；如果父进程很大，则复制可能需要很长时间；更糟糕的是，这项工作经常被大量浪费(子进程中的`fork()`后执行`exec()`将导致子进程丢弃复制的内存，可能不会使用其中的大部分).另一方面，如果父母和孩子都使用一个页面，并且一个或两个都写它，那么确实需要一个副本.

`cow fork()`的目标是推迟为子进程分配和复制物理内存页面，直到实际需要进行`copy`(进程需要对page进行写操作)

- `COW fork()`只为子级创建一个页表，用户内存的 PTE 指向父级的物理页面，且将`parent`和`child`中的所有用户 PTE 标记为不可写(清除PTE_W flag).当任一进程尝试写入这些 COW 页之一时，CPU 将强制发生page fault. Handler检测到这种情况，为出错进程分配物理内存页面，将原始页面复制到新页面中，并修改出错进程中的相关 PTE 以引用新页面，这次使用PTE 标记为可写(set PTE_W flag)。当页面错误处理程序返回时，用户进程将能够写入它的页面副本.
- 一个给定的物理页可以被多个进程的页表引用，并且只有在最后一个引用消失时才应该被释放(smart pointer ===> reference count)

**实现过程**

- 修改`uvmcopy()`以将父级的物理页面映射到子级，而不是分配新页面.在子级和父级的`PTE`中清除`PTE_W`

  ```C
  int 
  uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
  {
      pte_t *pte;
      uint64 pa, i;
      uint flags;
  
      acquire(&ref_cnt_lock);
      for(i = 0; i < sz; i += PGSIZE){
          if((pte = walk(old, i, 0)) == 0)
              panic("uvmcopy: pte should exist");
          if((*pte & PTE_V) == 0)
              panic("uvmcopy: page not present");
          pa = PTE2PA(*pte);
          if (*pte & PTE_W) { // if the page is writeable, then clear the PTE_W flag
              *pte |= PTE_COW;
              *pte &= ~PTE_W;
          }
          flags = PTE_FLAGS(*pte);
          if(mappages(new, i, PGSIZE, pa, flags) != 0) goto err;
          
          ++ref_cnt[(uint64)pa >> 12];
      }
      release(&ref_cnt_lock);
      return 0;
  
  err:
      uvmunmap(new, 0, i / PGSIZE, 1);
  
      release(&ref_cnt_lock);         
      return -1;
  }
  ```

- 修改 usertrap() 以识别页面错误。当 COW 页面发生缺页时，使用 kalloc() 分配新页面，将旧页面复制到新页面，并将新页面安装到 PTE 中并设置`PTE_W` 。

  ```C
  // trap.c
  if{
    // ...
  }else if (r_scause() == 15){
      uint64 va = r_stval();                                         
      int res = cow_handler(va, p->pagetable); 
      if (res < 0) p->killed = 1;
  } else if(/* ... */) {
    // ...
  } else{
    // ...
  }
  ```

- 确保每个物理页面在对它的最后一个 PTE 引用消失时被释放——但不是在此之前。做到这一点的一个好方法是为每个物理页保留一个“引用计数”，该“引用计数”是指引用该页的用户页表的数量。当`kalloc()`分配页面时，将页面的引用计数设置为 1 。当 fork 导致子共享页面时增加页面的引用计数，并在每次任何进程从其页表中删除页面时减少页面的计数。 `kfree()`如果其引用计数为零，则应仅将页面放回空闲列表中。可以将这些计数保存在固定大小的整数数组中。您必须制定一个方案来确定如何索引数组以及如何选择其大小。例如，您可以使用页的物理地址除以 4096 来索引数组，并为数组提供等于kalloc.c 中 `kinit()`放置在空闲列表中的任何页的最高物理地址的元素数。

  ```C
  int ref_cnt[PHYSTOP >> 12];
  struct spinlock ref_cnt_lock;
  ```

- 修改 copyout() 以在遇到 COW 页面时使用与页面错误相同的方案

  ```C
  int
  copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
  {
      uint64 n, va0, pa0;
      int res;
      while(len > 0){
          va0 = PGROUNDDOWN(dstva);
          res = cow_handler(va0, pagetable); // cow页处理
          if (res < 0)
              return -1;
          pa0 = walkaddr(pagetable, va0);
          if(pa0 == 0)
              return -1;
          n = PGSIZE - (dstva - va0);
          if(n > len)
              n = len;
          memmove((void *)(pa0 + (dstva - va0)), src, n);
  
          len -= n;
          src += n;
          dstva = va0 + PGSIZE;
      }
      return 0;
  }
  ```

**测试程序结果：**

**Note**：其中一个测试样例被注释掉了，后续将修复其中的bug

<img src="https://xmtxpic.oss-cn-hangzhou.aliyuncs.com/img/%E6%88%AA%E5%B1%8F2022-09-01%2012.20.24.png" alt="截屏2022-09-01 12.20.24" style="zoom:50%;" />