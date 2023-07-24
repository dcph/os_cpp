 

#ifndef __OSCPP__DRIVERS__VGA_H
#define __OSCPP__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace oscpp
{
    namespace drivers
    {
        
        class VGA//vga类，320*200大小，具有固定的内存位置
        {
        protected:
            hardwarecommunication::Port8Bit miscPort;//杂物
            hardwarecommunication::Port8Bit crtcIndexPort;//控制器
            hardwarecommunication::Port8Bit crtcDataPort;
            hardwarecommunication::Port8Bit sequencerIndexPort;//电子管程序
            hardwarecommunication::Port8Bit sequencerDataPort;
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;//图控制
            hardwarecommunication::Port8Bit graphicsControllerDataPort;
            hardwarecommunication::Port8Bit attributeControllerIndexPort;//属性控制
            hardwarecommunication::Port8Bit attributeControllerReadPort;
            hardwarecommunication::Port8Bit attributeControllerWritePort;
            hardwarecommunication::Port8Bit attributeControllerResetPort;
            
            void WriteRegisters(common::uint8_t* registers);//从寄存器开始初始化vga
            common::uint8_t* GetFrameBufferSegment();
            
            virtual common::uint8_t GetColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b);
            
            
        public:
            VGA();
            ~VGA();
            
            virtual bool SupportsMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);
            virtual bool SetMode(common::uint32_t width, common::uint32_t height, common::uint32_t colordepth);//初始化vga
            virtual void PutPixel(common::int32_t x, common::int32_t y,  common::uint8_t r, common::uint8_t g, common::uint8_t b);
            virtual void PutPixel(common::int32_t x, common::int32_t y, common::uint8_t colorIndex);
            
            virtual void FillRectangle(common::uint32_t x, common::uint32_t y, common::uint32_t w, common::uint32_t h, 
            common::uint8_t r, common::uint8_t g, common::uint8_t b);

        };
        
    }
}

#endif