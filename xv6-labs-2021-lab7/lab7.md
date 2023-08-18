# Lab 7 networking

### 实验目的
本实验旨在开发一个高效的网卡驱动程序，以实现数据包的传输和接收。我们将通过编写代码来初始化E1000网卡并实现`e1000_transmit`和`e1000_recv`函数，从而实现网络数据包的发送和接收。这些函数的具体实现将涉及`DMA`传输、数据包描述符等网络协议细节。

### 实验步骤
1. 在开始编码之前，仔细阅读已经提供的代码，特别是涉及到初始化和网卡操作的部分。这将有助于理解整个驱动程序的结构和工作原理。

2. 首先，需要实现`e1000_transmit`函数，该函数负责将数据包发送到网络。以下是该函数的主要代码，以及相关的解释：
```
int e1000_transmit(struct mbuf *m) {
    acquire(&e1000_lock);

    // 获取当前的发送缓冲区索引
    uint32 bufindex = regs[E1000_TDT];
    struct tx_desc *desc = &tx_ring[bufindex];

    // 检查是否有空闲的发送缓冲区
    if (!(desc->status & E1000_TXD_STAT_DD)) {
        release(&e1000_lock);
        return -1;
    }

    // 释放之前的数据缓冲区（如果有）
    if (tx_mbufs[bufindex]) {
        mbuffree(tx_mbufs[bufindex]);
        tx_mbufs[bufindex] = 0;
    }

    // 设置发送描述符
    desc->addr = (uint64)m->head;
    desc->length = m->len;
    desc->cmd = E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS;
    tx_mbufs[bufindex] = m;

    // 更新发送缓冲区索引
    regs[E1000_TDT] = (regs[E1000_TDT] + 1) % TX_RING_SIZE;

    release(&e1000_lock);

    return 0;
}
```

在这段代码中，我们首先获取当前的发送缓冲区索引，然后检查该缓冲区是否可用。如果可用，我们设置发送描述符并更新缓冲区索引。最后，我们释放之前的数据缓冲区并返回。

3. 接下来，实现`e1000_recv`函数，该函数负责接收并处理接收到的网络数据包。以下是该函数的主要代码，以及相关的解释：
```
static void e1000_recv(void) {
    while (1) {
        // 获取下一个接收缓冲区索引
        uint32 bufindex = (regs[E1000_RDT] + 1) % RX_RING_SIZE;
        struct rx_desc *desc = &rx_ring[bufindex];

        // 检查是否有接收到新的数据包
        if (!(desc->status & E1000_RXD_STAT_DD))
            return;

        // 设置接收数据包的长度并调用网络协议栈处理
        rx_mbufs[bufindex]->len = desc->length;
        net_rx(rx_mbufs[bufindex]);

        // 为下一个数据包准备接收缓冲区
        rx_mbufs[bufindex] = mbufalloc(0);
        desc->addr = (uint64)rx_mbufs[bufindex]->head;
        desc->status = 0;
        regs[E1000_RDT] = bufindex;
    }
}
```

在这段代码中，我们循环检查接收缓冲区是否有新的数据包到达。如果有，我们设置接收数据包的长度，并调用网络协议栈来处理数据。然后，我们为下一个数据包准备接收缓冲区，并更新接收缓冲区索引。

4. 完成`e1000_transmit`和`e1000_recv`函数的实现后，编译整个驱动程序并在`xv6`操作系统中进行测试。确保在虚拟机中打开两个终端窗口，一个用于运行`xv6`操作系统，另一个用于运行`nettests`以测试数据包的发送和接收功能。

### 实验中遇到的问题和解决办法
1. 问题：在实现`e1000_transmit`函数时，数据包可能会丢失或者发送错误，导致网络通信不稳定。
- 解决办法：
- 仔细检查发送描述符设置： 确保在设置发送描述符时，正确地指定数据包的地址、长度和命令等参数。
保证互斥访问： 确保在修改发送缓冲区时使用互斥锁，以防止多个线程同时访问发送缓冲区而导致混乱。
调试和日志记录： 在代码中加入调试打印语句或者日志记录，可以帮助你追踪问题发生的位置和原因。

2. 问题：在实现`e1000_recv`函数时，接收到的数据包可能没有被正确处理，导致网络数据无法正常传递给操作系统的协议栈。
- 解决办法：
- 检查接收描述符设置： 确保在设置接收描述符时，地址和状态等参数被正确设置，以便正确接收数据包。
- 验证网络协议栈调用： 确保在接收到数据包后，正确调用了网络协议栈的处理函数，以便处理接收到的数据。
- 排查数据传递问题： 如果数据包在接收后没有正确传递给协议栈，可以检查数据包的处理流程，确保数据被正确传递和处理。


### 实验心得
通过完成这个实验，我深刻理解了网络驱动程序的开发过程和底层细节。通过实现`e1000_transmit`和`e1000_recv`函数，我学会了如何控制数据包的发送和接收，以及如何管理发送和接收缓冲区。此外，我还学会了如何调试和排查网络驱动程序中可能遇到的问题，如数据包丢失等。

总的来说，这个实验不仅加深了我的操作系统和网络编程知识，还提升了我的编码和调试能力。我认为这些知识和经验对我未来的学习和职业发展将会有很大的帮助。

# 评分
```
== Test running nettests == 
$ make qemu-gdb
(8.1s) 
== Test   nettest: ping == 
  nettest: ping: OK 
== Test   nettest: single process == 
  nettest: single process: OK 
== Test   nettest: multi-process == 
  nettest: multi-process: OK 
== Test   nettest: DNS == 
  nettest: DNS: OK 
== Test time == 
time: OK 
Score: 100/100
```