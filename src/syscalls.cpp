
#include <syscalls.h>
 
using namespace oscpp;
using namespace oscpp::common;
using namespace oscpp::hardwarecommunication;
 
SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
:    InterruptHandler(interruptManager, InterruptNumber  + interruptManager->HardwareInterruptOffset())
{
}

SyscallHandler::~SyscallHandler()
{
}


void printf(char*);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;
    

    switch(cpu->eax)//查看编号
    {
        case 4:
            printf((char*)cpu->ebx);
            break;
            
        default:
            break;
    }

    
    return esp;
}

