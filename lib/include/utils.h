#pragma once

#include <cstdint>

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

#include "profiler.h"

bool isBigEndian();
bool isLittleEndian();
void write16ToBuffer( void* ptr, uint16_t value );
uint16_t read16FromBuffer( void * ptr );
void write32ToBuffer( void * ptr, uint32_t value );
uint32_t read32FromBuffer( void* ptr );
void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format = true, int symbol_per_line = 64 );
void printSet( std::set<uint32_t> myset );

template< class T >
void printVector(const std::vector< T >& v )
{
    for( int i = 0; i < v.size(); ++i )
    {
        std::cerr << (int)v[i] << " ";
    }
    std::cerr << std::endl;
}

template< class T >
void removeDups( std::vector< T >& vec )
{
#ifdef USE_PROFILING
    $
#endif
    std::sort( vec.begin(), vec.end() );
    vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
}

//bool isMulticastAddress(const std::string &ipAddress);
//std::string getNetworkInterfaceIP(const std::string &interfaceName);

void dumbXOR(uint8_t* a, uint8_t* b , int size);
