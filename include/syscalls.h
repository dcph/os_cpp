 
#ifndef __OSCPP__SYSCALLS_H
#define __OSCPP__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace oscpp
{
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {
        
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, oscpp::common::uint8_t InterruptNumber);
        ~SyscallHandler();
        
        virtual oscpp::common::uint32_t HandleInterrupt(oscpp::common::uint32_t esp);

    };
    
    
}


#endif