
#include <gdt.h>
using namespace oscpp;
using namespace oscpp::common;

//32位的线性地址和16位的边界，最大64kb的空间
GDT::GDT()
    : nullSegmentSelector(0, 0, 0),
        unusedSegmentSelector(0, 0, 0),
        codeSegmentSelector(0, 64*1024*1024, 0x9A),
        dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    uint32_t i[2];//GDTR需要信息
    i[1] = (uint32_t)this;//当前地址，32位
    i[0] = sizeof(GDT) << 16;//表长界限，16位
    asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));//内联汇编，访问初始地址，lgdt指令将GDT的入口地址装入GDTR寄存器
}

GDT::~GDT()
{
}

uint16_t GDT::DataSegmentSelector()
{
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}

uint16_t GDT::CodeSegmentSelector()
{
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}

GDT::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type)//基址base，32，段最大长度limit，20和段类型type
{
    // 获取全局描述符首地址
    uint8_t *target = (uint8_t *)this;//8，因为头两个是16位
    // 判断是否需要使用页作为最小单位
    // 如果段的字节数不足65536，则最小单位是字节，否则为页
    if (limit < 65536)
    {
        // 寻找到limit_hi字段
        // 将最小单位设置为字节
        // 使用32位段
        target[6] = 0x40;
    }
    else
    {
        // 32位地址空间。现在我们必须将（32位）限制压缩为2.5个寄存器（20位）。32位的空间转为20位，通过将最小的12位全部设置成1来解决，最小的12位就是页的内容
        // 将最小单位设置为页
        target[6] = 0xc0;
        // 如果最后4KB不足则需要缺失一页
        // 将limit转化为4KB的页面数
        if ((limit & 0xfff) != 0xfff)
            limit = (limit >> 12) - 1;
        else
            limit = limit >> 12;
    }
    // 此时limit是20位的
    // limit字段低8位取limit的后8位
    target[0] = limit & 0xff;
    // limit字段低16位的后8位
    target[1] = (limit >> 8) & 0xff;
    // limit字段高位
    target[6] = (limit >> 16) & 0xf | 0xc0;

    // base字段低16位的前8位
    target[2] = base & 0xff;
    // base字段低16位的后8位
    target[3] = (base >> 8) & 0xff;
    // base字段的次高位
    target[4] = (base >> 16) & 0xff; 
    // base字段的最高位
    target[7] = (base >> 24) & 0xff; 

    // type字段
    target[5] = type;
}

uint32_t GDT::SegmentDescriptor::Base()//找到base
{
    // 获取当前对象的首地址
    uint8_t *target = (uint8_t *)this;
    // 从最高位开始获取，并进行左移
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];

    return result;
}

uint32_t GDT::SegmentDescriptor::Limit()//找到limit
{
    uint8_t* target = (uint8_t*)this;

    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xFFF;

    return result;
}

