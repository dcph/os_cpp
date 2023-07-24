#ifndef __OSCPP__GDT_H
#define __OSCPP__GDT_H
// limit: 寻址最大范围
// flags: 标志位 
// access: 访问权限
// 用于记录段基址和段偏移
#include <common/types.h>

namespace oscpp
{
    
    class GDT//全局描述符表
    {
        public:

            class SegmentDescriptor//段描述符
            {
                private:
                    oscpp::common::uint16_t limit_lo;          // limit字段低位
                    oscpp::common::uint16_t base_lo;           // base字段低位
                    oscpp::common::uint8_t base_hi;            // base字段次高位
                    oscpp::common::uint8_t type;               // type字段和其他标志
                    oscpp::common::uint8_t limit_hi;           // limit字段高位
                    oscpp::common::uint8_t base_vhi;           // base字段最高位

                public:
                    SegmentDescriptor(oscpp::common::uint32_t base, oscpp::common::uint32_t limit, oscpp::common::uint8_t type);
                    oscpp::common::uint32_t Base();//返回基址
                    oscpp::common::uint32_t Limit();//返回段长度
            } __attribute__((packed));//取消结构在编译过程中的优化对齐

        private:
            SegmentDescriptor nullSegmentSelector;//空
            SegmentDescriptor unusedSegmentSelector;//未使用
            SegmentDescriptor codeSegmentSelector;//代码段
            SegmentDescriptor dataSegmentSelector;//数据段

        public:

            GDT();
            ~GDT();

            oscpp::common::uint16_t CodeSegmentSelector();//返回代码段相对于GDT表地址的偏移量
            oscpp::common::uint16_t DataSegmentSelector();//返回数据段相对于GDT表地址的偏移量
    };

}
    
#endif