#pragma once

#include <cstdint>

#include <memory>
#include <set>

#include "datablock.h"
#include "utils.h"

namespace trevi
{
class CodeBlock : public DataBlock
{
public:
    // Constructor from payload
    CodeBlock( uint16_t mtu, uint8_t * payloadBuffer, int payloadSize );

    // Constructor from raw buffer
    CodeBlock( uint8_t * rawBuffer, int rawBufferSize );

    virtual ~CodeBlock();

    virtual uint16_t payload_size();
    virtual void * payload_ptr();

    uint32_t get_stream_sequence_idx();
    void set_stream_sequence_idx(uint32_t value);

    void set_stream_id(uint8_t value);
    uint8_t get_stream_id();

    void updateCRC();
    uint16_t readCRC();
    bool isCorrectCRC();

    void setCompositionField(uint32_t offset, std::set< uint32_t > compoSet );
    void dumpCompositionField();
    void dumpCompositionBitset();

    std::string dumpCompositionFieldStr();
    std::set<uint32_t> getCompositionSet();

    int degree();

    std::shared_ptr< CodeBlock > clone();

    void XOR_payload( std::shared_ptr< CodeBlock > other );

    void dumpPayload()
    {
        trevi_print_bytes( std::cerr, "CodeBlock Payload", (const unsigned char*)payload_ptr(), payload_size() );
    }

private:
    void setPayloadSize(uint16_t plSize );

    void * footer_ptr();
    void setMNS();
    void setMNE();

    static const int CODEBLOCK_HEADER_SIZE = 20;
    static const int CODEBLOCK_STREAM_SEQ_IDX_OFFSET = 2;
    static const int CODEBLOCK_COMPO_OFFSET = 6;
    static const int CODEBLOCK_PAYLOAD_SIZE_OFFSET = 10;
    static const int CODEBLOCK_STREAM_ID_OFFSET = 14;
    static const int CODEBLOCK_FOOTER_SIZE = 4;
    static const uint16_t CODEBLOCK_MNS = 0x2609;
    static const uint16_t CODEBLOCK_MNE = 0x2804;

protected:

};
}
