//用于设置中断描述符表，中断描述符表(IDT)是保护模式下用于存储中断处理程序入口的表。表中所有描述符都记录一段程序的起始地址
//IDT的地址也放在一个特殊的寄存器idtr中，使用lidt指令进行装载
#ifndef __OSCPP__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H
#define __OSCPP__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H

#include <gdt.h>
#include <multitasking.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>


namespace oscpp
{
    namespace hardwarecommunication
    {

        class InterruptManager;//中断管理器

        class InterruptHandler//中断处理类
        {
            protected:
                oscpp::common::uint8_t InterruptNumber;
                InterruptManager* interruptManager;
                InterruptHandler(InterruptManager* interruptManager, oscpp::common::uint8_t InterruptNumber);
                ~InterruptHandler();
            public:
                virtual oscpp::common::uint32_t HandleInterrupt(oscpp::common::uint32_t esp);//中断处理程序
        };


        class InterruptManager
        {
            friend class InterruptHandler;
            protected:

                static InterruptManager* ActiveInterruptManager;//同一时刻只有一个中断
                InterruptHandler* handlers[256];//最多256个中断
                TaskManager *taskManager;//任务管理器
                //门描述符，一个中断源就会产生一个中断向量，一个中断向量对应IDT中的一个门描述符，通过门描述符可以找到对应的中断处理程序
                //中断门包含了中断处理程序所在段的段选择子和段内偏移地址
                //调用门提供给用户进程进入特权0级的方式
                //陷阱门与中断门类似
                struct GateDescriptor
                {
                    oscpp::common::uint16_t handlerAddressLowBits;//中断处理程序地址低位
                    oscpp::common::uint16_t gdt_codeSegmentSelector;//GDT代码段选择子
                    oscpp::common::uint8_t reserved;
                    oscpp::common::uint8_t access;//访问权限
                    oscpp::common::uint16_t handlerAddressHighBits;//中断处理程序地址高位
                } __attribute__((packed));

                static GateDescriptor IDT[256];//中断描述符表，最大为256//中断向量号将作为IDT的索引查找中断描述符

                struct IDTPointer//中断描述符表存放寄存器数据
                {
                    oscpp::common::uint16_t size;//16位数据
                    oscpp::common::uint32_t base;//32位地址
                } __attribute__((packed));

                oscpp::common::uint16_t hardwareInterruptOffset;//中断偏移量
                static void SetIDTEntry(oscpp::common::uint8_t interrupt, //设置中断描述符表
                    oscpp::common::uint16_t codeSegmentSelectorOffset, void (*handler)(),
                    oscpp::common::uint8_t DescriptorPrivilegeLevel, oscpp::common::uint8_t DescriptorType);//初始化中断描述符，代码段选择偏移、中断服务例程入口地址、特权级DPL、描述符类型type


                static void InterruptIgnore();

                static void HandleInterruptRequest0x00();//中断服务例程，调用中断处理程序
                static void HandleInterruptRequest0x01();
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();

                static void HandleInterruptRequest0x80();

                static void HandleException0x00();//异常服务例程
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();
                
                static oscpp::common::uint32_t HandleInterrupt(oscpp::common::uint8_t interrupt, oscpp::common::uint32_t esp);//中断处理程序，处理完成后需要返回，需要当前的栈顶指针esp
                oscpp::common::uint32_t DoHandleInterrupt(oscpp::common::uint8_t interrupt, oscpp::common::uint32_t esp);//执行中断处理

                Port8BitSlow programmableInterruptControllerMasterCommandPort;//可编程中断控制器主命令端口
                Port8BitSlow programmableInterruptControllerMasterDataPort;//主数据端口
                Port8BitSlow programmableInterruptControllerSlaveCommandPort;//从属命令端口
                Port8BitSlow programmableInterruptControllerSlaveDataPort;//从属数据端口

            public:
                InterruptManager(oscpp::common::uint16_t hardwareInterruptOffset, oscpp::GDT* globalDescriptorTable, oscpp::TaskManager* taskManager);//需要传入中断硬偏移，GDT，任务管理器
                ~InterruptManager();
                oscpp::common::uint16_t HardwareInterruptOffset();//记录偏移量
                void Activate();//用于开启中断
                void Deactivate();//用于关闭中断
        };
        
    }
}

#endif