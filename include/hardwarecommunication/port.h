
#ifndef __OSCPP__HARDWARECOMMUNICATION__PORT_H
#define __OSCPP__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace oscpp
{
    namespace hardwarecommunication//用于定义数据通信的端口
    {

        class Port
        {
            protected:
                Port(oscpp::common::uint16_t portnumber);
                // FIXME: Must be virtual (currently isnt because the kernel has no memory management yet)
                ~Port();
                oscpp::common::uint16_t portnumber;
        };

        //对端口的读写操作的汇编指令分别为in和out，后缀bwl表示端口的位数
        //asm用来声明一个内联汇编表达式
        //volatile声明不答应对该内联汇编优化
        class Port8Bit : public Port  //定义不同数据类型的port，不用类模板是因为需要汇编操作
        {
            public:
                Port8Bit(oscpp::common::uint16_t portnumber);
                ~Port8Bit();

                virtual oscpp::common::uint8_t Read();
                virtual void Write(oscpp::common::uint8_t data);

            protected:
                static inline oscpp::common::uint8_t Read8(oscpp::common::uint16_t _port)//设置为内联
                {
                    oscpp::common::uint8_t result;
                    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));//读取8bit数据
                    return result;
                }

                static inline void Write8(oscpp::common::uint16_t _port, oscpp::common::uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));//写入8bit数据
                }
        };



        class Port8BitSlow : public Port8Bit  //慢写操作
        {
            public:
                Port8BitSlow(oscpp::common::uint16_t portnumber);
                ~Port8BitSlow();

                virtual void Write(oscpp::common::uint8_t data);
            protected:
                static inline void Write8Slow(oscpp::common::uint16_t _port, oscpp::common::uint8_t _data)
                {
                    __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));//中间会经过两个jump来增加时间
                }

        };



        class Port16Bit : public Port
        {
            public:
                Port16Bit(oscpp::common::uint16_t portnumber);
                ~Port16Bit();

                virtual oscpp::common::uint16_t Read();
                virtual void Write(oscpp::common::uint16_t data);

            protected:
                static inline oscpp::common::uint16_t Read16(oscpp::common::uint16_t _port)
                {
                    oscpp::common::uint16_t result;
                    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write16(oscpp::common::uint16_t _port, oscpp::common::uint16_t _data)
                {
                    __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };



        class Port32Bit : public Port
        {
            public:
                Port32Bit(oscpp::common::uint16_t portnumber);
                ~Port32Bit();

                virtual oscpp::common::uint32_t Read();
                virtual void Write(oscpp::common::uint32_t data);

            protected:
                static inline oscpp::common::uint32_t Read32(oscpp::common::uint16_t _port)
                {
                    oscpp::common::uint32_t result;
                    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                    return result;
                }

                static inline void Write32(oscpp::common::uint16_t _port, oscpp::common::uint32_t _data)
                {
                    __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd" (_port));
                }
        };

    }
}


#endif
