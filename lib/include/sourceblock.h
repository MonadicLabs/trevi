#pragma once

#include "datablock.h"

class SourceBlock : public DataBlock
{
public:
    // From payload
    SourceBlock( uint8_t* payloadBuffer, int payloadSize );

    // From Raw Buffer
    SourceBlock( int bufferSize, uint8_t* rawBuffer );

    virtual ~SourceBlock();

    virtual uint16_t payload_size();
    virtual void * payload_ptr();

    uint32_t get_global_sequence_idx();
    void set_global_sequence_idx(uint32_t value);

    virtual void * footer_ptr();
    void updateCRC();
    uint16_t readCRC();
    uint16_t computeCRC();
    bool isCorrectCRC();

private:
    static const int SOURCEBLOCK_HEADER_SIZE = 6;
    static const int SOURCEBLOCK_GLOBAL_SEQ_IDX_OFFSET = 2;
    static const int SOURCEBLOCK_FOOTER_SIZE = 2;
    void setPayloadSize( uint16_t plSize );

protected:

};
