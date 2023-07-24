
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <syscalls.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

#include <drivers/amd_am79c973.h>


// #define GRAPHICSMODE


using namespace oscpp;
using namespace oscpp::common;
using namespace oscpp::drivers;
using namespace oscpp::hardwarecommunication;
using namespace oscpp::gui;



void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;//显卡的调用地址

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];//高四位是色彩，低四位是内容
                x++;
                break;
        }

        if(x >= 80)//重开一行
        {
            x = 0;
            y++;
        }

        if(y >= 25)//上移一行
        {
            for(y = 0; y < 24; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | (VideoMemory[80*(y+1)+x] & 0xFF);
            VideoMemory[80*24] = (VideoMemory[80*24] & 0xFF00) | str[i];
            for(x = 1; x < 80; x++)
                 VideoMemory[80*24+x] = (VideoMemory[80*24+x] & 0xFF00) | ' ';
            x = 1;
            y = 24;
        }
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}
void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}





class PrintfKeyboardEventHandler : public KeyboardEventHandler//打印键盘结果
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF); //所在位置内容不变，反色       
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;//防止越界
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};




void sysprintf(char* str)
{
    asm("int $0x80" : : "a" (4), "b" (str));//中断指令，中断号为80
}

void taskA()
{
    while(true)
        sysprintf("A");
}

void taskB()
{
    while(true)
        sysprintf("B");
}






typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()//用于初始化.init_array段的变量
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t multiboot_magic)//用于接受eax和ebx中的数据
{
    printf("os_cpp by dch\n");

    GDT gdt;
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);//起始地址为bootloader地址+8
    size_t heap = 10*1024*1024;//堆大小
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);//堆后是栈
    
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);
    
    void* allocated = memoryManager.malloc(1024);//内存分配测试
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");
    
    TaskManager taskManager;//多任务测试
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);

    
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    SyscallHandler syscalls(&interrupts, 0x80);//设置syscall中断为80
    
    printf("Initializing Hardware, Stage 1\n");
    
    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);//GUI测试
    #endif
    
    DriverManager drvManager;
    
        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);//将键盘中断加入中断处理器
        #endif
        drvManager.AddDriver(&keyboard);
        
    
        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);//将鼠标中断加入中断处理器
        #endif
        drvManager.AddDriver(&mouse);
        
        PCIController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);//PCI加载所有驱动

        #ifdef GRAPHICSMODE
            VGA vga;
        #endif
        
    printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();//启动所有驱动
        
    printf("Initializing Hardware, Stage 3\n");

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif

    //磁盘检测
    // printf("\nS-ATA primary master: ");
    // ATA ata0m(true, 0x1F0);
    // ata0m.Identify();
    
    // printf("\nS-ATA primary slave: ");
    // ATA ata0s(false, 0x1F0);
    // ata0s.Identify();
    // ata0s.Write28(0, (uint8_t*)"oscpp", 25);
    // ata0s.Flush();
    // ata0s.Read28(0, 25);
    
    // printf("\nS-ATA secondary master: ");
    // ATA ata1m(true, 0x170);
    // ata1m.Identify();
    
    // printf("\nS-ATA secondary slave: ");
    // ATA ata1s(false, 0x170);
    // ata1s.Identify();
    // // third: 0x1E8
    // // fourth: 0x168
    
    
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);//网络测试
    eth0->Send((uint8_t*)"Hello World", 11);
        

    interrupts.Activate();//启动中断


    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}
