
#include "../include/utils.h"

#include <iomanip>
#include <iostream>
#include <set>

#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

using namespace std;

#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

bool isLittleEndian()
{
    int n = 1;
    return (*(char *)&n == 1);
}

bool isBigEndian()
{
    return !isLittleEndian();
}


void write16ToBuffer(void *ptr, uint16_t value)
{
    uint8_t * bufferPtr = (uint8_t*)ptr;
    uint16_t nValue = htons( value );
    uint8_t partA = (uint8_t)((nValue & 0xFF00) >> 8);
    uint8_t partB = (uint8_t)(nValue & 0x00FF);
    bufferPtr[0] = partA;
    bufferPtr[1] = partB;
}

uint16_t read16FromBuffer(void *ptr)
{
    uint8_t* bufferPtr = (uint8_t*)ptr;
    uint8_t partA = bufferPtr[0];
    uint8_t partB = bufferPtr[1];
    uint16_t wd = ((uint16_t)partA << 8) | partB;
    return htons( wd );
}


void write32ToBuffer(void *ptr, uint32_t value)
{
    uint32_t nValue = htonl( value );
    memcpy(ptr, (const char *) &(nValue), sizeof(uint32_t) );
}


uint32_t read32FromBuffer(void *ptr)
{
    uint8_t* bufferPtr = (uint8_t*)ptr;
    uint32_t nValue = (((uint32_t) bufferPtr[0]) << 0 |
                                                    ((uint32_t) bufferPtr[1]) << 8 |
                                                                                 ((uint32_t) bufferPtr[2]) <<  16 |
                                                                                                               ((uint32_t) bufferPtr[3]) <<  24);
    return ntohl( nValue );
}

void trevi_print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format, int symbol_per_line )
{
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % symbol_per_line == 0) ? "\n" : " ");
        }
    }
    out << std::endl;
}


void printSet(std::set<uint32_t> myset)
{
    cerr << "set.size()=" << myset.size() << endl;
    std::set<uint32_t>::iterator it;
    for (it=myset.begin(); it!=myset.end(); ++it)
    {
        int idx = *it;
        cerr << idx << " ";
    }
    cerr << endl;
}

void dumbXOR(uint8_t *a, uint8_t *b, int size )
{
    for( int i = 0; i < size; ++i )
    {
        a[ i ] = a[ i ] ^ b[i];
    }
}

#ifdef __arm__
void memcpy_neon_align32( char *dst, const char *src, int count )
{
    int   remain;
    char* fdst;
    const char* fsrc;

    remain = count;
    fdst   = dst;
    fsrc   = src;

    while( 32 <= remain )
    {
        asm volatile (
                    ".fpu  neon\n"
                    "1:                                               \n"
                    "subs     %[remain], %[remain], #32               \n"
                    "vld1.u8  {d0, d1, d2, d3}, [%[fsrc],:256]!       \n"
                    "vst1.u8  {d0, d1, d2, d3}, [%[fdst],:256]!       \n"
                    "bgt      1b                                      \n"
                    : [fsrc]"+r"(fsrc), [fdst]"+r"(fdst), [remain]"+r"(remain)
                    :
                    : "d0", "d1", "d2", "d3", "cc", "memory"
                    );
    }
}
#endif

void trevi_memcpy(void *dst, const void *src, size_t num)
{
//#ifdef __arm__
//    cerr << "is_aligned(src)=" << is_aligned(src, 32) << " is_aligned(dst)=" << is_aligned(dst, 32) << endl;
//    if( is_aligned(dst,32) && is_aligned(src,32) )
//    {
//        cerr << "neon_memcpy" << endl;
//        memcpy_neon_align32( dst, src, num );
//    }
//    else
//    {
//        cerr << "std memcpy" << endl;
//        return memcpy(dst,src, num);
//    }
//#else
    memcpy( dst, src, num );
//#endif
}

void *trevi_malloc(size_t s)
{
//#ifdef __arm__
//    return aligned_alloc( 32, s );
//#else
    return malloc(s);
//#endif
}
