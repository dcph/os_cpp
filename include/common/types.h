#ifndef __OSCPP__COMMON__TYPES_H
#define __OSCPP__COMMON__TYPES_H

namespace oscpp
{
    namespace common
    {

        typedef char                     int8_t; //用于统一标识，因为不同机子上int的长度不同
        typedef unsigned char           uint8_t;
        typedef short                   int16_t;
        typedef unsigned short         uint16_t;
        typedef int                     int32_t;
        typedef unsigned int           uint32_t;
        typedef long long int           int64_t;
        typedef unsigned long long int uint64_t;
    
        typedef const char*              string;
        typedef uint32_t                 size_t;
    }
}
    
#endif