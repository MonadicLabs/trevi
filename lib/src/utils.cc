
#include "utils.h"

#include <iomanip>
#include <iostream>

#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

using namespace std;

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

void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format, int symbol_per_line )
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
