#pragma once

#include <cstdint>

#include <memory>

#include "codeblock.h"
#include "sourceblock.h"
#include "ogesolver.h"
#include "reorderingbuffer.h"

namespace trevi {
class Decoder;
class StreamDecoder
{
    friend class Decoder;

public:
    StreamDecoder( uint16_t decodingWindowSize );
    virtual ~StreamDecoder();

    void addCodeBlock( uint8_t* buffer, int bufferSize );
    void addCodeBlock( std::shared_ptr<trevi::CodeBlock> cb );

    bool available();
    std::shared_ptr< trevi::SourceBlock > pop();

private:
    int _curMinSeqIdx;
    int _curMaxSeqIdx;
    int _infPacketIdxCount;

    int _decodingWindowSize;

    std::shared_ptr<OGESolver> _oge;
    std::shared_ptr<ReorderingBuffer> _buffer;

    Decoder * _parent;
    void setParent( Decoder * parent);

protected:

};
}
