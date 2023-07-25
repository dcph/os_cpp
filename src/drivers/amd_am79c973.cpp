
#include <drivers/amd_am79c973.h>
using namespace oscpp;
using namespace oscpp::common;
using namespace oscpp::drivers;
using namespace oscpp::hardwarecommunication;

// 初始化流程
// 1. 从 PCI 驱动程序获取并注册驱动和端口
// 2.找出MAC地址;这在16x0（APROM00、APROM0 和 APROM2）的三个 4 位寄存器中以小端顺序提供。
// 3.重置卡。
// 4.执行停止重置。通过将0x04写入 CSR0 来实现的。
// 5.初始化发送和接收缓冲区;两者都必须大小为 2 kB 且物理连续。
// 6.初始化块
// 7.初始化接受和发送描述符
// 8.注册初始化块

amd_am79c973::amd_am79c973(PCIDeviceDescriptor *dev, InterruptManager* interrupts)
:   Driver(),
    InterruptHandler(interrupts, dev->interrupt + interrupts->HardwareInterruptOffset()),//初始化中断
    MACAddress0Port(dev->portBase),
    MACAddress2Port(dev->portBase + 0x02),
    MACAddress4Port(dev->portBase + 0x04),
    registerDataPort(dev->portBase + 0x10),
    registerAddressPort(dev->portBase + 0x12),
    resetPort(dev->portBase + 0x14),
    busControlRegisterDataPort(dev->portBase + 0x16)//初始化端口
{
    currentSendBuffer = 0;
    currentRecvBuffer = 0;
    
    uint64_t MAC0 = MACAddress0Port.Read() % 256;//获取48位MAC地址
    uint64_t MAC1 = MACAddress0Port.Read() / 256;
    uint64_t MAC2 = MACAddress2Port.Read() % 256;
    uint64_t MAC3 = MACAddress2Port.Read() / 256;
    uint64_t MAC4 = MACAddress4Port.Read() % 256;
    uint64_t MAC5 = MACAddress4Port.Read() / 256;
    
    uint64_t MAC = MAC5 << 40
                 | MAC4 << 32
                 | MAC3 << 24
                 | MAC2 << 16
                 | MAC1 << 8
                 | MAC0;
    
    registerAddressPort.Write(20);//启动
    busControlRegisterDataPort.Write(0x102);//将软件样式更改为PCnet-PCI，启动32位
    
    registerAddressPort.Write(0);
    registerDataPort.Write(0x04);//停止重置
    
    sendBufferDescr = (BufferDescriptor*)((((uint32_t)&sendBufferDescrMemory[0]) + 15) & ~((uint32_t)0xF));//初始化接受发送缓冲区，设置为2kb大小
    initBlock.sendBufferDescrAddress = (uint32_t)sendBufferDescr;
    recvBufferDescr = (BufferDescriptor*)((((uint32_t)&recvBufferDescrMemory[0]) + 15) & ~((uint32_t)0xF));
    initBlock.recvBufferDescrAddress = (uint32_t)recvBufferDescr;
    
    initBlock.mode = 0x0000; //初始化块
    initBlock.reserved1 = 0;
    initBlock.numSendBuffers = 3;
    initBlock.reserved2 = 0;
    initBlock.numRecvBuffers = 3;
    initBlock.physicalAddress = MAC;
    initBlock.reserved3 = 0;
    initBlock.logicalAddress = 0;

    // 对于接收描述符，必须在“flags”字段中设置OWN位（0x80000000），并且“flags”字段的位0到11也必须设置为0x7FF（缓冲区的负长度，即-2048）。
    // 此外，必须在两种描述符类型的“标志”字段中设置位 12 到 15 （0x0000F000）。八个接收描述符必须一个接一个地写入初始化块的“receive_descriptor”字段中输入的缓冲区。
    // 同样，必须将八个发送描述符写入在“transmit_descriptor”字段中输入的缓冲区。
    for(uint8_t i = 0; i < 8; i++)//初始化接受发送描述符
    {
        sendBufferDescr[i].address = (((uint32_t)&sendBuffers[i]) + 15 ) & ~(uint32_t)0xF;//指定2kB缓冲区的物理地址
        sendBufferDescr[i].flags = 0x7FF
                                 | 0xF000;
        sendBufferDescr[i].flags2 = 0;
        sendBufferDescr[i].avail = 0;
        
        recvBufferDescr[i].address = (((uint32_t)&recvBuffers[i]) + 15 ) & ~(uint32_t)0xF;
        recvBufferDescr[i].flags = 0xF7FF
                                 | 0x80000000;
        recvBufferDescr[i].flags2 = 0;
        sendBufferDescr[i].avail = 0;
    }
    

    registerAddressPort.Write(1);//注册初始化块
    registerDataPort.Write(  (uint32_t)(&initBlock) & 0xFFFF );
    registerAddressPort.Write(2);
    registerDataPort.Write(  ((uint32_t)(&initBlock) >> 16) & 0xFFFF );
    
}

amd_am79c973::~amd_am79c973()
{
}
            
void amd_am79c973::Activate()
{
    registerAddressPort.Write(0);
    registerDataPort.Write(0x41);//初始化卡并激活中断

    registerAddressPort.Write(4);
    uint32_t temp = registerDataPort.Read();
    registerAddressPort.Write(4);
    registerDataPort.Write(temp | 0xC00);//自动将小于 64 字节的数据包放大到该大小，并从收到的数据包中删除一些相对多余的信息
    
    registerAddressPort.Write(0);
    registerDataPort.Write(0x42);//允许卡投入运行
}

int amd_am79c973::Reset()
{
    resetPort.Read();
    resetPort.Write(0);
    return 10;
}


void printf(char*);
void printfHex(uint8_t);

uint32_t amd_am79c973::HandleInterrupt(common::uint32_t esp)
{
    printf("INTERRUPT FROM AMD am79c973\n");
    
    registerAddressPort.Write(0);
    uint32_t temp = registerDataPort.Read();//读取数据，发生中断时，CSR0 寄存器会显示中断原因
    
    if((temp & 0x8000) == 0x8000) printf("AMD am79c973 ERROR\n");
    if((temp & 0x2000) == 0x2000) printf("AMD am79c973 COLLISION ERROR\n");
    if((temp & 0x1000) == 0x1000) printf("AMD am79c973 MISSED FRAME\n");
    if((temp & 0x0800) == 0x0800) printf("AMD am79c973 MEMORY ERROR\n");
    if((temp & 0x0400) == 0x0400) Receive();
    if((temp & 0x0200) == 0x0200) printf("AMD am79c973 DATA SENT\n");
                               
    registerAddressPort.Write(0);
    registerDataPort.Write(temp);// 处理结束时，应将 CSR0 寄存器的内容写回该寄存器，以向卡指示中断已处理
    
    if((temp & 0x0100) == 0x0100) printf("AMD am79c973 INIT DONE\n");
    
    return esp;
}

       
void amd_am79c973::Send(uint8_t* buffer, int size)
{
    int sendDescriptor = currentSendBuffer;
    currentSendBuffer = (currentSendBuffer + 1) % 8;//将相应数据缓冲区的物理地址写入下一个发送描述符的地址字段中
    
    if(size > 1518)
        size = 1518;//发送边界
    
    for(uint8_t *src = buffer + size -1,
                *dst = (uint8_t*)(sendBufferDescr[sendDescriptor].address + size -1);
                src >= buffer; src--, dst--)
        *dst = *src;//将数据放入发送缓冲区中
    
    sendBufferDescr[sendDescriptor].avail = 0;
    sendBufferDescr[sendDescriptor].flags2 = 0;//指示发送时是否发生错误
    sendBufferDescr[sendDescriptor].flags = 0x8300F000
                                          | ((uint16_t)((-size) & 0xFFF));
    registerAddressPort.Write(0);
    registerDataPort.Write(0x48);//保持中断处于激活状态
}

void amd_am79c973::Receive()
{
    printf("AMD am79c973 DATA RECEIVED\n");
    
    for(; (recvBufferDescr[currentRecvBuffer].flags & 0x80000000) == 0;//从当前描述符处理所有接收描述符，直到 OWN 位设置为0，也就是未读取
        currentRecvBuffer = (currentRecvBuffer + 1) % 8)
    {
        if(!(recvBufferDescr[currentRecvBuffer].flags & 0x40000000)
         && (recvBufferDescr[currentRecvBuffer].flags & 0x03000000) == 0x03000000)//正常接收到的数据
        
        {
            uint32_t size = recvBufferDescr[currentRecvBuffer].flags & 0xFFF;
            if(size > 64)
                size -= 4;
            
            uint8_t* buffer = (uint8_t*)(recvBufferDescr[currentRecvBuffer].address);//找到缓冲区地址
            
            for(int i = 0; i < size; i++)
            {
                printfHex(buffer[i]);//写出数据
                printf(" ");
            }
        }
        
        recvBufferDescr[currentRecvBuffer].flags2 = 0;
        recvBufferDescr[currentRecvBuffer].flags = 0x8000F7FF;//设置为已读取
    }
}

