//键盘驱动程序
#ifndef __OSCPP__DRIVERS__KEYBOARD_H
#define __OSCPP__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace oscpp
{
    namespace drivers
    {
    
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };
        
        class KeyboardDriver : public oscpp::hardwarecommunication::InterruptHandler, public Driver //继承中断处理父类
        {
            oscpp::hardwarecommunication::Port8Bit dataport;//数据端
            oscpp::hardwarecommunication::Port8Bit commandport;//命令端
            
            KeyboardEventHandler* handler;
        public:
            KeyboardDriver(oscpp::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler *handler);
            ~KeyboardDriver();
            virtual oscpp::common::uint32_t HandleInterrupt(oscpp::common::uint32_t esp);//虚继承处理中断函数
            virtual void Activate();//启动驱动
        };

    }
}
    
#endif