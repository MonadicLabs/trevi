
#include "sourceblock.h"
#include "utils.h"
#include "crc16.h"

#include <cstring>

#include <iostream>

using namespace std;

SourceBlock::SourceBlock(uint8_t *payloadBuffer, int payloadSize)
{
    initMemory( payloadSize + SOURCEBLOCK_HEADER_SIZE + SOURCEBLOCK_FOOTER_SIZE );
    memcpy( payload_ptr(), payloadBuffer, payloadSize );
    setPayloadSize( payloadSize );
}

SourceBlock::SourceBlock(int bufferSize, uint8_t *rawBuffer)
{
    initMemory( bufferSize );
    memcpy( buffer_ptr(), rawBuffer, bufferSize );
}

SourceBlock::~SourceBlock()
{

}

uint16_t SourceBlock::payload_size()
{
    uint8_t* plSizePtr = (uint8_t*)buffer_ptr();
    return read16FromBuffer( plSizePtr );
}

void *SourceBlock::payload_ptr()
{
    return (void*)((uint8_t*)(buffer_ptr()) + SOURCEBLOCK_HEADER_SIZE);
}

uint32_t SourceBlock::get_global_sequence_idx()
{
    uint8_t* gidxPtr = (uint8_t*)buffer_ptr() + SOURCEBLOCK_GLOBAL_SEQ_IDX_OFFSET;
    return read32FromBuffer( gidxPtr );
}

void SourceBlock::set_global_sequence_idx(uint32_t value)
{
    uint8_t* gidxPtr = (uint8_t*)buffer_ptr() + SOURCEBLOCK_GLOBAL_SEQ_IDX_OFFSET;
    return write32ToBuffer( gidxPtr, value );
}

void *SourceBlock::footer_ptr()
{
    return (void*)((uint8_t*)buffer_ptr() + SOURCEBLOCK_HEADER_SIZE + payload_size() );
}

void SourceBlock::updateCRC()
{
    uint16_t crcValue = computeCRC();
    write16ToBuffer( footer_ptr(), crcValue );
}

uint16_t SourceBlock::readCRC()
{
    return read16FromBuffer( footer_ptr() );
}

uint16_t SourceBlock::computeCRC()
{
    uint16_t crcValue = crc16( (const char*)buffer_ptr(), payload_size() + SOURCEBLOCK_HEADER_SIZE );
    return crcValue;
}

bool SourceBlock::isCorrectCRC()
{
    uint16_t rValue = readCRC();
    uint16_t cValue = computeCRC();
    return rValue == cValue;
}

void SourceBlock::setPayloadSize(uint16_t plSize)
{
    uint8_t* plSizePtr = (uint8_t*)buffer_ptr();
    write16ToBuffer( plSizePtr, plSize );
}
