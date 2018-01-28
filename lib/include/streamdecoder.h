#pragma once

#include <cstdint>

#include <memory>

#include "codeblock.h"
#include "sourceblock.h"
#include "ogesolver.h"
#include "reorderingbuffer.h"

class Decoder;
class StreamDecoder
{
    friend class Decoder;

public:
    StreamDecoder( uint16_t decodingWindowSize );
    virtual ~StreamDecoder();

    void addCodeBlock( uint8_t* buffer, int bufferSize );
    void addCodeBlock( std::shared_ptr<CodeBlock> cb );

    bool available();
    std::shared_ptr< SourceBlock > pop();

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
