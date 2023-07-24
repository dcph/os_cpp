
#include <multitasking.h>

using namespace oscpp;
using namespace oscpp::common;


Task::Task(GDT *gdt, void entrypoint())
{
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));//找到新的4mb存放任务
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    cpustate -> eip = (uint32_t)entrypoint;//记录程序入口
    cpustate -> cs = gdt->CodeSegmentSelector();//代码段偏移量
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
    
}

Task::~Task()
{
}

        
TaskManager::TaskManager()
{
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    tasks[numTasks++] = task;
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)//没有任务
        return cpustate;
    
    if(currentTask >= 0)//存在当前任务
        tasks[currentTask]->cpustate = cpustate;//记录当前任务状态
    
    if(++currentTask >= numTasks)//任务切换
        currentTask %= numTasks;
    return tasks[currentTask]->cpustate;//切换到下一任务状态
}

    