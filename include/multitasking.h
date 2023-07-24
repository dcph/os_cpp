 //多任务处理
#ifndef __OSCPP__MULTITASKING_H
#define __OSCPP__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace oscpp
{
    
    struct CPUState //记录cpu中寄存器的状态
    {
        common::uint32_t eax;
        common::uint32_t ebx;
        common::uint32_t ecx;
        common::uint32_t edx;

        common::uint32_t esi;
        common::uint32_t edi;
        common::uint32_t ebp;
        // common::uint32_t gs;
        // common::uint32_t fs;
        // common::uint32_t es;
        // common::uint32_t ds;
        common::uint32_t error;

        common::uint32_t eip;
        common::uint32_t cs;
        common::uint32_t eflags;
        common::uint32_t esp;
        common::uint32_t ss;        
    } __attribute__((packed));
    
    
    class Task//任务类
    {
    friend class TaskManager;
    private:
        common::uint8_t stack[4096]; //栈大小，4MB
        CPUState* cpustate;
    public:
        Task(GDT *gdt, void entrypoint());
        ~Task();
    };
    
    
    class TaskManager//任务管理类
    {
    private:
        Task* tasks[256];//最多256个任务
        int numTasks;
        int currentTask;
    public:
        TaskManager();
        ~TaskManager();
        bool AddTask(Task* task);
        CPUState* Schedule(CPUState* cpustate);
    };
    
    
    
}


#endif