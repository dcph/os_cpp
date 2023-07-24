//ATA端口，用于硬盘驱动器
#ifndef __OSCPP__DRIVERS__ATA_H
#define __OSCPP__DRIVERS__ATA_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace oscpp
{
    namespace drivers
    { 
        class ATA
        {
        protected:
            bool master;//主从硬盘驱动器标识
            hardwarecommunication::Port16Bit dataPort;//数据端口
            hardwarecommunication::Port8Bit errorPort;//错误端口
            hardwarecommunication::Port8Bit sectorCountPort;//扇区端口
            hardwarecommunication::Port8Bit lbaLowPort;//逻辑块端口
            hardwarecommunication::Port8Bit lbaMidPort;
            hardwarecommunication::Port8Bit lbaHiPort;
            hardwarecommunication::Port8Bit devicePort;//模式端口
            hardwarecommunication::Port8Bit commandPort;//指令端口
            hardwarecommunication::Port8Bit controlPort;//数据端口
        public:
            
            ATA(bool master, common::uint16_t portBase);
            ~ATA();
            
            void Identify();
            void Read28(common::uint32_t sectorNum, int count = 512);
            void Write28(common::uint32_t sectorNum, common::uint8_t* data, common::uint32_t count);
            void Flush();//磁盘刷新

        };
    }
}

#endif
