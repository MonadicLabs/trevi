#pragma once

#include <map>

#include "streamdecoder.h"
#include "decodeoutput.h"
#include "reorderingbuffer.h"

namespace trevi{

class Decoder
{
public:
    Decoder();
    virtual ~Decoder();

    void addStream(int streamId, StreamDecoder* decoder);
    void removeStream( int streamId );

    void addCodeBlock( uint8_t* buffer, int bufferSize );
    void addCodeBlock( std::shared_ptr<trevi::CodeBlock> cb );

    void onNewDecodeOutput( const trevi::DecodeOutput& deco );

    bool available();
    std::shared_ptr< trevi::SourceBlock > pop();

    void clear()
    {
        _buffer->clear();
    }

private:

    std::deque< DecodeOutput > _output;

    std::shared_ptr< ReorderingBuffer > _buffer;
    std::map< uint8_t, StreamDecoder * > _decoders;

protected:

};
}
