//网卡驱动器
#ifndef __OSCPP__DRIVERS__AMD_AM79C973_H
#define __OSCPP__DRIVERS__AMD_AM79C973_H


#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/pci.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>


namespace oscpp
{
    namespace drivers
    {
        
        class amd_am79c973 : public Driver, public hardwarecommunication::InterruptHandler
        {
            struct InitializationBlock//初始化块信息
            {
                common::uint16_t mode;
                unsigned reserved1 : 4;
                unsigned numSendBuffers : 4;//发送数量
                unsigned reserved2 : 4;
                unsigned numRecvBuffers : 4;//收取数量 
                common::uint64_t physicalAddress : 48;//MAC地址
                common::uint16_t reserved3;
                common::uint64_t logicalAddress;
                common::uint32_t recvBufferDescrAddress;//2kb缓冲区物理地址
                common::uint32_t sendBufferDescrAddress;
            } __attribute__((packed));
            
            
            struct BufferDescriptor//接受与发送缓冲区描述符
            {
                common::uint32_t address;//指定2 kB缓冲区的物理地址
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;
            } __attribute__((packed));
            
            hardwarecommunication::Port16Bit MACAddress0Port;
            hardwarecommunication::Port16Bit MACAddress2Port;
            hardwarecommunication::Port16Bit MACAddress4Port;
            hardwarecommunication::Port16Bit registerDataPort;
            hardwarecommunication::Port16Bit registerAddressPort;
            hardwarecommunication::Port16Bit resetPort;
            hardwarecommunication::Port16Bit busControlRegisterDataPort;
            
            InitializationBlock initBlock;
            
            
            BufferDescriptor* sendBufferDescr;
            common::uint8_t sendBufferDescrMemory[2048+15];//2kb大小
            common::uint8_t sendBuffers[2048+15][8];
            common::uint8_t currentSendBuffer;
            
            BufferDescriptor* recvBufferDescr;
            common::uint8_t recvBufferDescrMemory[2048+15];
            common::uint8_t recvBuffers[2048+15][8];
            common::uint8_t currentRecvBuffer;
            
            
        public:
            amd_am79c973(oscpp::hardwarecommunication::PCIDeviceDescriptor *dev,
                         oscpp::hardwarecommunication::InterruptManager* interrupts);
            ~amd_am79c973();
            
            void Activate();
            int Reset();
            common::uint32_t HandleInterrupt(common::uint32_t esp);
            
            void Send(common::uint8_t* buffer, int count);
            void Receive();
        };
        
        
        
    }
}



#endif
