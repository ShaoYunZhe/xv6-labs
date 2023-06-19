# lab3

全部改动见[commit lab3](https://github.com/ShaoYunZhe/xv6-labs/commit/eadfe72ab23167278f82db4e3c1f9f7ff87bbcf4)

## Speed up system calls

当我们调用getpid时，实际上是进入内核，查看PCB中记录的pid，然后返回。事实上，因为pid是进程被创建时就确定下来的，因此其实可以在用户空间中分配一个所谓的“用户内核共享页”对pid进行备份。之后需要查看pid时，就无需切换到内核了。

具体实现方式是：

- 定义用户逻辑地址空间中usyscall的结构和地址，这件事设计者已经帮我们做好了。

- 在`proc.h`中进程控制块的结构体定义中新建一项，用于记录所谓的共享页的地址。

```
struct usyscall *usyscall;
```

- 在`proc.c`中修改新建和结束进程的函数，使其兼容usyscall，大体来说有四步：
  - `allocproc`申请内存空间，并将pid写入usyscall对应的空间中。
  - `proc_pagetable`设置用户进程的地址映射
  - `proc_freepagetable`解除用户进程的地址映射
  - `freeproc`释放内存空间
  - 执行的顺序很重要，应当先分配内存，再设置映射。先解除映射，再释放内存。当初做的时候这个bug困扰我多时！！！



## Print a page table 

只要理解pagetable采用的是多级索引，就可以把本问题转化成一个索引树上的深度优先遍历。

通过PTE的标志位可以判断递归是否终止：

- 如果PTE_V==0表示这个节点没用上，直接返回
- 否则打印该节点的信息
  - 如果PTE_W|PTE_R|PTE_X==0表示该节点是叶子节点了
  - 否则继续访问子节点



## Detect which pages have been accessed

个人认为是最简单的一个问题，不知道为什么难度为hard。

只需要理解PTE_A是访问位，对每个页面检查这一位是否为1即可。如果为1，则需要将访问位设为0（因为pgaccess考察的是上次检查之后，有哪些页面被新调用过），然后更新位图即可。归0和位图的构造可以用位运算来轻松得到。

```c
if(*pte & PTE_A)
{
	ret |= (1<<i);
	*pte &= (~PTE_A);
}
```