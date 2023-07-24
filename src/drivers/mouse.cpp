
#include <drivers/mouse.h>


using namespace oscpp::common;
using namespace oscpp::drivers;
using namespace oscpp::hardwarecommunication;


    void printf(char*);

    MouseEventHandler::MouseEventHandler()
    {
    }
    
    void MouseEventHandler::OnActivate()
    {
    }
    
    void MouseEventHandler::OnMouseDown(uint8_t button)
    {
    }
    
    void MouseEventHandler::OnMouseUp(uint8_t button)
    {
    }
    
    void MouseEventHandler::OnMouseMove(int x, int y)
    {
    }





    MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
    : InterruptHandler(manager, 0x2C),//中断向量号
    dataport(0x60),//控制器端口
    commandport(0x64)//数据端口
    {
        this->handler = handler;
    }

    MouseDriver::~MouseDriver()
    {
    }
    
    void MouseDriver::Activate()
    {
        offset = 0;
        buttons = 0;

        if(handler != 0)
            handler->OnActivate();
        
        commandport.Write(0xA8);
        commandport.Write(0x20); // 读取控制器命令字节
        uint8_t status = dataport.Read() | 2;
        commandport.Write(0x60); // 设置控制器命令字节
        dataport.Write(status);

        commandport.Write(0xD4);
        dataport.Write(0xF4);
        dataport.Read();        
    }
    
    uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
    {
        uint8_t status = commandport.Read();
        if (!(status & 0x20))// 如果是键盘操作，那么直接返回 
            return esp;

        buffer[offset] = dataport.Read();//按偏移量记录当前信息
        
        if(handler == 0)
            return esp;
        
        offset = (offset + 1) % 3;

        if(offset == 0)//当offset为0时，需要更新鼠标状态
        {
            if(buffer[1] != 0 || buffer[2] != 0)//处理坐标信息
            {
                handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
            }

            for(uint8_t i = 0; i < 3; i++)
            {
                if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))//处理鼠标状态
                {
                    if(buttons & (0x1<<i))
                        handler->OnMouseUp(i+1);
                    else
                        handler->OnMouseDown(i+1);
                }
            }
            buttons = buffer[0];
        }
        
        return esp;
    }
