# lab2

全部改动见[commit-lab2](https://github.com/ShaoYunZhe/xv6-labs/commit/1e9d4991c0667fcd166b58061ac44380f5ea0d0c)

首先梳理一下系统调用的流程：

- `user/sh.c`解析用户输入的指令和参数，然后调用相应用户指令处理程序如`user/sleep.c`，然后调用相应的系统调用接口

- `user/user.h`中定义了若干系统调用的声明，或者说是接口
- `user/usys.S`中给出了这些接口的伪实现

```assembly
.global fork
fork:
 li a7, SYS_fork
 ecall
 ret
```

`.global`关键字将这个过程与`user/user.h`中的接口链接起来。SYS_fork是fork函数的编号，定义在`kernal/syscall.h`中。这段代码的含义其实是，将系统调用的编号存入`a7`寄存器中，然后调用`ecall`指令触发系统中断。

于是我们得以解决lab1中留下的疑问：为什么用vscode找不到系统调用的实现？原因是这样的：因为需要调用`ecall`指令，不得不写汇编代码，于是就需要将c语言和汇编用`.global`关键字链接起来，而vscode的代码提示功能比较笨，所以没能找到系统调用的（伪）实现。

对于每一个系统调用，无非是函数名和编号不同，因此可以写一个脚本`user/usys.pl`，来生成这些汇编代码。

- `kernal/syscall.c`文件中的syscall函数通过读取a7寄存器，知道用户调用了哪个指令，然后通过syscalls这个函数指针数组，调用相应的处理程序
- `kernal/sysproc.c`最终给出了所有系统调用的实现



## System call tracing

该功能希望实现：通过用户给出的mask，打印出执行某命令时调用某个系统调用的信息。根据上述调用流程，我们依次修改相应文件：

- `user/sh.c`和`user/trace.c`已经写好
- `user/user.h`中声明trace的接口
- `user/usys.pl`中添加`entry("trace")`
- `kernal/sysproc.c`中实现trace，只需要将mask加入进程控制块proc结构体中
  - 为此，还需要修改`kernal/proc.h`和`kernal/proc.c`
- `kernal/syscall.c`中每次执行系统调用时，与mask比对一下，如果一致则输出调用信息



## Sysinfo 

该功能希望实现：统计空闲内存和系统中的进程数量

这其实是两件事情，我们分别解决：

- `kernal/kalloc.c`中定义freemem()来统计空闲内存。xv6采用页式存储，空闲页记录在链表kmem.freelist中。计算链表的长度即可知道空闲页数，最后返回pg_num*PGSIZE。
- `kernal/proc.c`中定义nproc()来统计进程数。系统中的所有进程用结构体数组来管理，每个元素是一个进程控制块。只需要遍历数组中，状态不为UNUSED的进程控制块即可。
- `kernal/defs.h`中添加上述函数的声明
- 类似前一问的方式，修改各文件以实现系统调用