//内存管理
#ifndef __OSCPP__MEMORYMANAGEMENT_H
#define __OSCPP__MEMORYMANAGEMENT_H

#include <common/types.h>


namespace oscpp
{
    
    struct MemoryChunk//双链表管理内存
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;//是否使用
        common::size_t size;//内存块大小
    };
    
    
    class MemoryManager//内存管理类
    {
        
    protected:
        MemoryChunk* first;
    public:
        
        static MemoryManager *activeMemoryManager;//同样单例设计
        
        MemoryManager(common::size_t first, common::size_t size);
        ~MemoryManager();
        
        void* malloc(common::size_t size);
        void free(void* ptr);
    };
}


void* operator new(unsigned size);
void* operator new[](unsigned size);

void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);


#endif