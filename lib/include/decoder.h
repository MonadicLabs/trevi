#pragma once

#include <map>

#include "streamdecoder.h"
#include "decodeoutput.h"
#include "reorderingbuffer.h"

class Decoder
{
public:
    Decoder();
    virtual ~Decoder();

    void addStream(int streamId, StreamDecoder* decoder);
    void removeStream( int streamId );

    void addCodeBlock( uint8_t* buffer, int bufferSize );
    void addCodeBlock( std::shared_ptr<CodeBlock> cb );

    void onNewDecodeOutput( const DecodeOutput& deco );

    bool available();
    std::shared_ptr< SourceBlock > pop();

private:
    std::shared_ptr< ReorderingBuffer > _buffer;
    std::map< uint8_t, StreamDecoder * > _decoders;

protected:

};
