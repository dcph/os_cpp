#BootLoader,用于加载GRUB，需要遵循Multiboot规范
#由此进入kernel运行部分
#多引导项头部分

.set MAGIC, 0x1badb002          #魔术块
.set FLAGS, (1<<0 | 1<<1)       #标志块
.set CHECKSUM, -(MAGIC + FLAGS) #校验块

# 下面的伪指令声明了Multiboot标准中的多引导项头
# 三个块都是32位字段

.section .multiboot             #.section，汇编指示
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

#程序入口点
.section .text
.extern kernelMain              #声明kernelMain函数
.extern callConstructors        #初始化全局变量
.global loader                  #告知程序入口点，对外暴露loader函数


loader:
    mov $kernel_stack, %esp     #设置栈顶指针
    call callConstructors
    push %eax                   #压栈bootloader的地址
    push %ebx                   #压栈魔术数
    call kernelMain


_stop:                          #设置进入无限循环
    cli                         #禁用中断
    hlt                         #禁用中断后使用hlt暂停CPU，以后无法再唤醒
    jmp _stop


.section .bss
.space 2*1024*1024;             #2 MiB，栈顶指针在这之后，保证只是用2mb的空间
kernel_stack:                   

