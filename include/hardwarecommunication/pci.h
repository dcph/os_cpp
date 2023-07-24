//通信用PCI总线，PCI总线上只允许有一个PCI主设备，其他的均为PCI从设备，而且读写操作只能在主从设备之间进行，从设备之间的数据交换需要通过主设备中转。
// 总线通常包括一套控制线路和一套数据线路。控制线路用来传输请求和确认信号，并指出数据线上的信息类型。总线的数据线在源和目的之间传递信息。
// 这种信息可能包括数据、复杂指令或者地址。
//pci有设备存储器地址空间、I/O地址空间和配置空间，因为其即插即用的特性，PCI设备不占用固定的内存地址空间或I/O地址空间，而是由操作系统决定其映射的基址

// PCI总线规范定义的配置空间总长度为256个字节，配置信息按一定的顺序和大小依次存放。前64个字节的配置空间称为配置头，对于所有的设备都一样，
// 配置头的主要功能是用来识别设备、定义主机访问PCI卡的方式（I/O访问或者存储器访问，还有中断信息）。其余的192个字节称为本地配置空间（设备有关区），
// 主要定义卡上局部总线的特性、本地空间基地址及范围等

//PCI有256条总线，每条总线可以连接32个设备，每个设备最多有8个功能

#ifndef __OSCPP__HARDWARECOMMUNICATION__PCI_H
#define __OSCPP__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

#include <memorymanagement.h>

namespace oscpp
{
    namespace hardwarecommunication
    {

        enum BaseAddressRegisterType//基址寄存器
        {
            MemoryMapping = 0,//系统软件需要在内存映射I/O空间中为该资源分配地址
            InputOutput = 1//系统软件需要在I/O空间中为该资源分配地址
        };
             
        // 基址寄存器的第 0 位用于表示 PCI 设备内部资源的类型。PCI 设备制造商在生产该设备时会根据实际情况配置该值。对于系统软件来说，该位是只读的。
        // bit 0 = 1 :  系统软件需要在I/O空间中为该资源分配地址
        // bit 0 = 0 :  系统软件需要在内存映射I/O空间中为该资源分配地址
        // 两者分别简记为I/O类型和MMIO类型。如果资源是MMIO类型，则第1-3位有效。
        // 前两位为：00表示系统软件需要为该资源分配一个32位的地址10表示系统软件需要为该资源分配一个64为的地址
        // 如果需要分配64位的地址，相邻两个基址寄存器会合并成1个使用，后者保存64位地址的高32位。
        // 第3位为：1 表示该资源是可预取的0 表示该资源是不可预取的
        class BaseAddressRegister//基址寄存器类
        {
        public:
            bool prefetchable;
            oscpp::common::uint8_t* address;//配置空间起始地址
            oscpp::common::uint32_t size;//基址寄存器大小
            BaseAddressRegisterType type;
        };
        
        
        
        class PCIDeviceDescriptor//pci设备的配置空间
        {
        public:
            oscpp::common::uint32_t portBase;
            oscpp::common::uint32_t interrupt;
            
            oscpp::common::uint16_t bus;
            oscpp::common::uint16_t device;
            oscpp::common::uint16_t function;

            oscpp::common::uint16_t vendor_id;
            oscpp::common::uint16_t device_id;
            
            oscpp::common::uint8_t class_id;
            oscpp::common::uint8_t subclass_id;
            oscpp::common::uint8_t interface_id;

            oscpp::common::uint8_t revision;
            
            PCIDeviceDescriptor();
            ~PCIDeviceDescriptor();
            
        };


        class PCIController//控制类
        {
            Port32Bit dataPort;
            Port32Bit commandPort;
            
        public:
            PCIController();
            ~PCIController();
            //bus，device，function，哪条PCI总线、哪个设备、哪个功能，registeroffset，所要读取的字段在配置空间中的偏移量
            oscpp::common::uint32_t Read(oscpp::common::uint16_t bus, oscpp::common::uint16_t device, oscpp::common::uint16_t function,
             oscpp::common::uint32_t registeroffset);//访问配置空间
            void Write(oscpp::common::uint16_t bus, oscpp::common::uint16_t device, oscpp::common::uint16_t function, oscpp::common::uint32_t
             registeroffset, oscpp::common::uint32_t value);//数据写入配置空间
            bool DeviceHasFunctions(oscpp::common::uint16_t bus, oscpp::common::uint16_t device);//判断设备是否存在多功能
            
            //每个设备都有驱动，驱动都与中断相连
            void SelectDrivers(oscpp::drivers::DriverManager* driverManager, oscpp::hardwarecommunication::InterruptManager* interrupts);//扫描所有空间
            oscpp::drivers::Driver* GetDriver(PCIDeviceDescriptor dev, oscpp::hardwarecommunication::InterruptManager* interrupts);//获取设备对应驱动
            PCIDeviceDescriptor GetDeviceDescriptor(oscpp::common::uint16_t bus, oscpp::common::uint16_t device, oscpp::common::uint16_t function);//获取特定设备配置空间
            BaseAddressRegister GetBaseAddressRegister(oscpp::common::uint16_t bus, oscpp::common::uint16_t device,
             oscpp::common::uint16_t function, oscpp::common::uint16_t bar);//获取基址寄存器
        };

    }
}
    
#endif