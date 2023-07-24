//鼠标控制程序
#ifndef __OSCPP__DRIVERS__MOUSE_H
#define __OSCPP__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace oscpp
{
    namespace drivers
    {
    
        class MouseEventHandler//鼠标类
        {
        public:
            MouseEventHandler();

            virtual void OnActivate();//启动
            virtual void OnMouseDown(oscpp::common::uint8_t button);//按下
            virtual void OnMouseUp(oscpp::common::uint8_t button);//松开
            virtual void OnMouseMove(int x, int y);//移动
        };
        
        
        class MouseDriver : public oscpp::hardwarecommunication::InterruptHandler, public Driver//驱动
        {
            oscpp::hardwarecommunication::Port8Bit dataport;
            oscpp::hardwarecommunication::Port8Bit commandport;
            oscpp::common::uint8_t buffer[3];//包含了鼠标的位置（后2位）和状态信息（第1位）
            oscpp::common::uint8_t offset;//记录当前读取的是哪一位的信息
            oscpp::common::uint8_t buttons;//记录鼠标当前的状态，如是否是按下状态

            MouseEventHandler* handler;//中断处理
        public:
            MouseDriver(oscpp::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual oscpp::common::uint32_t HandleInterrupt(oscpp::common::uint32_t esp);
            virtual void Activate();
        };

    }
}
    
#endif
