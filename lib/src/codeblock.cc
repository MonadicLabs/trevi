#include "codeblock.h"
#include "crc16.h"
#include "gf256.h"

#include <cstring>

#include <iostream>
#include <bitset>
#include <sstream>

using namespace std;

// Constructor from payload
CodeBlock::CodeBlock(uint16_t mtu, uint8_t *payloadBuffer, int payloadSize)
{
    initMemory( CODEBLOCK_HEADER_SIZE + mtu + CODEBLOCK_FOOTER_SIZE );

    memcpy( payload_ptr(), payloadBuffer, payloadSize );
    setPayloadSize( payloadSize );
    setMNS();
    setMNE();
}

CodeBlock::CodeBlock(uint8_t *rawBuffer, int rawBufferSize)
{
    initMemory( rawBufferSize );
    memcpy( buffer_ptr(), rawBuffer, rawBufferSize );
}

CodeBlock::~CodeBlock()
{

}

uint16_t CodeBlock::payload_size()
{
    uint8_t* plSizePtr = (uint8_t*)buffer_ptr() + CODEBLOCK_PAYLOAD_SIZE_OFFSET;
    return read16FromBuffer( plSizePtr );
}

void *CodeBlock::payload_ptr()
{
    return (void*)((uint8_t*)(buffer_ptr()) + CODEBLOCK_HEADER_SIZE);
}

uint32_t CodeBlock::get_stream_sequence_idx()
{
    uint8_t* sidxPtr = (uint8_t*)buffer_ptr() + CODEBLOCK_STREAM_SEQ_IDX_OFFSET;
    return read32FromBuffer( sidxPtr );
}

void CodeBlock::set_stream_sequence_idx(uint32_t value)
{
    uint8_t* sidxPtr = (uint8_t*)buffer_ptr() + CODEBLOCK_STREAM_SEQ_IDX_OFFSET;
    return write32ToBuffer( sidxPtr, value );
}

void CodeBlock::set_stream_id(uint8_t value)
{
    ((uint8_t*)buffer_ptr())[ CODEBLOCK_STREAM_ID_OFFSET ] = value;
}

uint8_t CodeBlock::get_stream_id()
{
    return ((uint8_t*)buffer_ptr())[ CODEBLOCK_STREAM_ID_OFFSET ];
}

void CodeBlock::updateCRC()
{
    uint16_t crcValue = crc16( (const char*)buffer_ptr(), buffer_size() - CODEBLOCK_FOOTER_SIZE );
    write16ToBuffer( footer_ptr(), crcValue );
}

uint16_t CodeBlock::readCRC()
{
    return read16FromBuffer( footer_ptr() );
}

bool CodeBlock::isCorrectCRC()
{
    uint16_t rValue = readCRC();
    uint16_t cValue = crc16( (const char*)buffer_ptr(), buffer_size() - CODEBLOCK_FOOTER_SIZE );
    return rValue == cValue;
}

void CodeBlock::setCompositionField(uint32_t offset, std::set<uint32_t> compoSet)
{
    std::bitset<32> b(0x00000000);
    std::set<uint32_t>::iterator it;
    for (it=compoSet.begin(); it!=compoSet.end(); ++it)
    {
        int idx = *it;
        b.set(idx,1);
    }
    uint32_t compoValue = b.to_ulong();
    write32ToBuffer( ((uint8_t*)buffer_ptr() + CODEBLOCK_COMPO_OFFSET), compoValue );
    set_stream_sequence_idx( offset );
}

void CodeBlock::dumpCompositionField()
{
    uint32_t compoValue = read32FromBuffer( (uint8_t*)buffer_ptr() + CODEBLOCK_COMPO_OFFSET );
    uint32_t offset = get_stream_sequence_idx();
    std::bitset<32> b(compoValue);
    for( int i = 0; i < 32; ++i )
    {
        if( b[i] )
        {
            cerr << i + offset << " ";
        }
    }
    cerr << endl;
}

string CodeBlock::dumpCompositionFieldStr()
{
    std::stringstream sstr;
    uint32_t compoValue = read32FromBuffer( (uint8_t*)buffer_ptr() + CODEBLOCK_COMPO_OFFSET );
    uint32_t offset = get_stream_sequence_idx();
    std::bitset<32> b(compoValue);
    for( int i = 0; i < 32; ++i )
    {
        if( b[i] )
        {
            sstr << i + offset << " ";
        }
    }
    return sstr.str();
}

std::set<uint32_t> CodeBlock::getCompositionSet()
{
    std::set<uint32_t> ret;
    uint32_t compoValue = read32FromBuffer( (uint8_t*)buffer_ptr() + CODEBLOCK_COMPO_OFFSET );
    uint32_t offset = get_stream_sequence_idx();
    std::bitset<32> b(compoValue);
    for( int i = 0; i < 32; ++i )
    {
        if( b[i] )
        {
            ret.insert( i + offset );
        }
    }
    return ret;
}

int CodeBlock::degree()
{
    uint32_t compoValue = read32FromBuffer( (uint8_t*)buffer_ptr() + CODEBLOCK_COMPO_OFFSET );
    std::bitset<32> b(compoValue);
    return b.count();
}

std::shared_ptr<CodeBlock> CodeBlock::clone()
{
    return std::make_shared< CodeBlock >( (uint8_t*)(this->buffer_ptr()), this->buffer_size() );
}

void CodeBlock::XOR_payload(std::shared_ptr<CodeBlock> other)
{
    int blockSize = max( payload_size(), other->payload_size() );
    uint8_t tmpBuffer_A[ blockSize ];
    uint8_t tmpBuffer_B[ blockSize ];
    memset( tmpBuffer_A, 0, blockSize );
    memset( tmpBuffer_B, 0, blockSize );
    memcpy( tmpBuffer_A, payload_ptr(), payload_size() );
    memcpy( tmpBuffer_B, other->payload_ptr(), other->payload_size() );
    gf256_add_mem( tmpBuffer_A, tmpBuffer_B, blockSize );
    // dumbXOR( tmpBuffer_A, tmpBuffer_B, other->payload_size() );
    memcpy( payload_ptr(), tmpBuffer_A, payload_size() );
}

void CodeBlock::setPayloadSize(uint16_t plSize)
{
    uint8_t* plSizePtr = (uint8_t*)buffer_ptr() + CODEBLOCK_PAYLOAD_SIZE_OFFSET;
    write16ToBuffer( plSizePtr, plSize );
}

void *CodeBlock::footer_ptr()
{
    return (void*)((uint8_t*)buffer_ptr() + _bufferSize - CODEBLOCK_FOOTER_SIZE);
}

void CodeBlock::setMNS()
{
    write16ToBuffer( buffer_ptr(), CODEBLOCK_MNS );
}

void CodeBlock::setMNE()
{
    write16ToBuffer( (uint8_t*)footer_ptr() + 2, CODEBLOCK_MNE );
}
