 
#include <memorymanagement.h>

using namespace oscpp;
using namespace oscpp::common;


MemoryManager* MemoryManager::activeMemoryManager = nullptr;
        
MemoryManager::MemoryManager(size_t start, size_t size)
{
    activeMemoryManager = this;
    
    if(size < sizeof(MemoryChunk))//内存大小小于链表表头大小，那么无法分配
    {
        first = nullptr;
    }
    else
    {
        first = (MemoryChunk*)start;
        
        first -> allocated = false;
        first -> prev = nullptr;
        first -> next = nullptr;
        first -> size = size - sizeof(MemoryChunk);//可用内存大小
    }
}

MemoryManager::~MemoryManager()
{
    if(activeMemoryManager == this)
        activeMemoryManager = nullptr;
}
//|链表指向位置，节点内存|返回位置，使用内存|节点内存|使用内存|        
void* MemoryManager::malloc(size_t size)
{
    MemoryChunk *result = nullptr;
    
    for(MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next)
        if(chunk->size > size && !chunk->allocated)//分配足够大小内存
            result = chunk;
        
    if(result == 0)
        return 0;
    
    if(result->size >= size + sizeof(MemoryChunk) + 1)//分配内存过大
    {
        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        
        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);//下一个空闲内存
        temp->prev = result;
        temp->next = result->next;
        if(temp->next != 0)
            temp->next->prev = temp;
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));//返回未使用内存部分
}

void MemoryManager::free(void* ptr)
{
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk -> allocated = false;
    
    if(chunk->prev != 0 && !chunk->prev->allocated)//查找前后是否有能合并的空间
    {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if(chunk->next != 0)
            chunk->next->prev = chunk->prev;
        
        chunk = chunk->prev;
    }
    
    if(chunk->next != 0 && !chunk->next->allocated)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if(chunk->next != 0)
            chunk->next->prev = chunk;
    }
    
}




void* operator new(unsigned size)
{
    if(oscpp::MemoryManager::activeMemoryManager == 0)
        return 0;
    return oscpp::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](unsigned size)
{
    if(oscpp::MemoryManager::activeMemoryManager == 0)
        return 0;
    return oscpp::MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new(unsigned size, void* ptr)
{
    return ptr;
}

void* operator new[](unsigned size, void* ptr)
{
    return ptr;
}

void operator delete(void* ptr)
{
    if(oscpp::MemoryManager::activeMemoryManager != 0)
        oscpp::MemoryManager::activeMemoryManager->free(ptr);
}

void operator delete[](void* ptr)
{
    if(oscpp::MemoryManager::activeMemoryManager != 0)
        oscpp::MemoryManager::activeMemoryManager->free(ptr);
}