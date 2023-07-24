

.set IRQ_BASE, 0x20     #中断编号从20开始

.section .text

.extern _ZN4oscpp21hardwarecommunication16InterruptManager15HandleInterruptEhj

#宏定义
.macro HandleException num  #形参
.global _ZN4oscpp21hardwarecommunication16InterruptManager19HandleException\num\()Ev      #将代码对外发布
_ZN4oscpp21hardwarecommunication16InterruptManager19HandleException\num\()Ev:             #代码段
    movb $\num, (interruptnumber)  #加载异常号
    jmp int_bottom  #进入处理程序
.endm


.macro HandleInterruptRequest num
.global _ZN4oscpp21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4oscpp21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm


HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

HandleInterruptRequest 0x80


int_bottom:

    # 保存寄存器
    #pusha
    #pushl %ds
    #pushl %es
    #pushl %fs
    #pushl %gs
    
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # 加载寄存器
    #cld
    #mov $0x10, %eax
    #mov %eax, %eds
    #mov %eax, %ees

    # 调用c++函数
    pushl %esp
    push (interruptnumber)
    call _ZN4oscpp21hardwarecommunication16InterruptManager15HandleInterruptEhj
    #add %esp, 6
    mov %eax, %esp # 堆栈切换

    # restore registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
    #pop %gs
    #pop %fs
    #pop %es
    #pop %ds
    #popa
    
    add $4, %esp

.global _ZN4oscpp21hardwarecommunication16InterruptManager15InterruptIgnoreEv
_ZN4oscpp21hardwarecommunication16InterruptManager15InterruptIgnoreEv:

    iret #结束


.data  
    interruptnumber: .byte 0
