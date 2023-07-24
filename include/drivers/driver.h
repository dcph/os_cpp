 
#ifndef __OSCPP__DRIVERS__DRIVER_H
#define __OSCPP__DRIVERS__DRIVER_H

namespace oscpp
{
    namespace drivers
    {
        //驱动程序类
        class Driver
        {
        public:
            Driver();
            ~Driver();
            
            virtual void Activate();//激活
            virtual int Reset();//重置
            virtual void Deactivate();//停止工作
        };

        class DriverManager//驱动程序管理类
        {
        public:
            Driver* drivers[265];//最多265个
            int numDrivers;
            
        public:
            DriverManager();
            void AddDriver(Driver*);//增加
            
            void ActivateAll();//全开启
            
        };
        
    }
}
    
    
#endif