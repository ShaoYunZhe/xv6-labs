# lab1
全部改动见[commit-lab1](https://github.com/ShaoYunZhe/xv6-labs/commit/0112aae333df48b858410351b04e53f2a458c255)

操作系统内核提供了一些系统调用，声明见user/user.h文件，在本次实验中只需要为待实现的每个功能找到相关的系统调用，在程序中显式地调用它们即可。

一个常见的疑惑是，这些系统调用只有声明而找不到定义。如果用vscode的搜索功能或者跳转功能，会发现没有搜索结果。这是怎么回事呢？这个问题要等到lab2才能解决！我们目前先把这些函数当成黑盒子。



## sleep
[sleep.c](https://github.com/ShaoYunZhe/xv6-labs/blob/master/Lab1_Xv6%20and%20Unix%20utilities/xv6_for_Lab1/user/sleep.c)

略



## pingpong
[pingpong.c](https://github.com/ShaoYunZhe/xv6-labs/blob/master/Lab1_Xv6%20and%20Unix%20utilities/xv6_for_Lab1/user/pingpong.c)

使用fork创建子进程，父子进程使用 pipe 通信。pipe指令新建了一个通道，它由操作系统维护，可以通过文件描述符访问相应端口。父子进程的文件描述符均记录在p中，因此访问到的是同一条管道。

此题常见做法是建立两个管道，事实上一个管道就足够，但是要注意细节。父进程向管道中写入后应当调用wait，原因是有可能导致父进程自己写完，立马自己读出来，子进程的read一直阻塞着。直接的结果是只打印了pong而没有ping。事实上，如果不加wait语句，有约80%概率通过测试，这取决于cpu的调度顺序，如果刚好执行完父进程的write就切换到子进程，是有可能实现正确效果的。



## primes
[primes.c](https://github.com/ShaoYunZhe/xv6-labs/blob/master/Lab1_Xv6%20and%20Unix%20utilities/xv6_for_Lab1/user/primes.c)

递归函数filter(int fd)负责实现这样一个功能：从文件描述符中读取若干个数字，筛去和第一个数字互质的数，然后用通道传递给子进程。返回条件是fd所代表的那个管道读端口为空。

子进程怎样才能知道父进程已经把数字写入完毕了呢？写完所有数据以后，写进程需要主动调用close(p)关闭写端口，于是读进程就可以接收到一个类似\0的符号。否则，子进程永远不能确定父进程已经没有数字可传了，还会傻傻地阻塞着。

还有一个值得说明的点，就是wait(0)函数的使用。事实上这个通信模型不需要让父进程等待子进程返回，每个父进程把筛完的数据写进管道之后就可以结束进程了。这里调用wait(0)主要是因为：1、当第一个进程结束，bash就会继续接收指令，直观的结果是输出中间穿插了一个\$符号（类似于下面形式）；2、测试脚本也会以第一个进程的结束作为标志，因此可能接收不到完整的输出。上述两点的根本原因应该与exec函数的实现逻辑有关，但时间原因就没有继续探究。

```
init: starting sh
$ primes
prime 2
prime 3
$ prime 7
prime 11
```



## find
[find.c](https://github.com/ShaoYunZhe/xv6-labs/blob/master/Lab1_Xv6%20and%20Unix%20utilities/xv6_for_Lab1/user/find.c)

本质是一个树的遍历问题：打开一个文件，通过fstat()函数获取其类型

- 如果是文件（叶子节点），则对比文件名
- 如果是目录（内部节点），则依次访问目录内的每个文件。但要注意排除目录文件默认包含的.和..路径



## xargs
[xargs.c](https://github.com/ShaoYunZhe/xv6-labs/blob/master/Lab1_Xv6%20and%20Unix%20utilities/xv6_for_Lab1/user/xargs.c)

本质是一个字符串拼接处理的问题。

```
<command1> | xargs <command2>
```

command1的结果通过管道传给xargs命令，于是对于xargs来说，相当于文件描述符0中有如下形式的字符串：

```
<arg11> <arg12>
<arg21> <arg22> <arg23>
<arg31>
```

然后将每一行拼接到command后面，上面的例子经xargs命令后等价于如下三条指令：

```
<command2> <arg11> <arg12>
<command2> <arg21> <arg22> <arg23>
<command2> <arg31>
```

于是可以将整个问题分解为三部分：

- 读取前一个指令的输出：由于管道是随写随读的，因此不能保证一次性读完，也不能保证读到哪里就中断了。高效的做法应该是随写随读随处理，但是我只采用简单做法，先while循环不断读取，直到检测到写端口关闭。
- 解析和拼接字符串：扫描输入串，遇到空格则分词，遇到回车则调用exec执行然后清空参数。这里只需要理解exec接受的参数是一个字符串和一个指针数组。字符串表明了要执行的指令，指针数组中每个指针指向一个字符串表示参数，直到读取到第一个空指针为止。按照上述规则组织填写exec的参数即可。
- 执行指令：不可以直接在主进程中调用exec，因为exec的原理其实是修改当前进程的控制块，进程中原有的指令再也不会执行了。因此需要exec时，应当fork一个子进程。





