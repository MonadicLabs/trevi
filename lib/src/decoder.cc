
#include "decoder.h"

Decoder::Decoder()
{
    _buffer = std::make_shared<ReorderingBuffer>(64);
}

Decoder::~Decoder()
{

}

void Decoder::addStream(int streamId, StreamDecoder *decoder)
{
    _decoders.insert( std::make_pair( streamId, decoder ) );
    decoder->setParent( this );
    return;
}

void Decoder::removeStream(int streamId)
{
    auto kv = _decoders.find( streamId );
    if( kv != _decoders.end() )
    {
        _decoders.erase( kv );
    }
}

void Decoder::addCodeBlock(uint8_t *buffer, int bufferSize)
{
    std::shared_ptr<CodeBlock> cb = std::make_shared<CodeBlock>( buffer, bufferSize );
    addCodeBlock( cb );
}

void Decoder::addCodeBlock(std::shared_ptr<CodeBlock> cb)
{
    uint8_t streamId = cb->get_stream_id();
    for( auto kv : _decoders )
    {
        if( kv.first == streamId )
        {
            kv.second->addCodeBlock( cb );
        }
    }
}

void Decoder::onNewDecodeOutput(const DecodeOutput &deco)
{
    _buffer->addBlock( deco.stream_idx, deco.global_idx, deco.block );
}

bool Decoder::available()
{
    return _buffer->available();
}

std::shared_ptr<SourceBlock> Decoder::pop()
{
    return _buffer->pop();
}
