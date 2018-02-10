
#include "decoder.h"

using namespace trevi;

trevi::Decoder::Decoder()
{
    _buffer = std::make_shared<ReorderingBuffer>(64);
}

trevi::Decoder::~Decoder()
{

}

void trevi::Decoder::addStream(int streamId, StreamDecoder *decoder)
{
    _decoders.insert( std::make_pair( streamId, decoder ) );
    decoder->setParent( this );
    return;
}

void trevi::Decoder::removeStream(int streamId)
{
    auto kv = _decoders.find( streamId );
    if( kv != _decoders.end() )
    {
        _decoders.erase( kv );
    }
}

void trevi::Decoder::addCodeBlock(uint8_t *buffer, int bufferSize)
{
    std::shared_ptr<trevi::CodeBlock> cb = std::make_shared<trevi::CodeBlock>( buffer, bufferSize );
    addCodeBlock( cb );
}

void trevi::Decoder::addCodeBlock(std::shared_ptr<trevi::CodeBlock> cb)
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

void trevi::Decoder::onNewDecodeOutput(const trevi::DecodeOutput &deco)
{
    _buffer->addBlock( deco.stream_idx, deco.global_idx, deco.block );
    // _output.push_back( deco );
}

bool trevi::Decoder::available()
{
    return _buffer->available();
    // return _output.size() > 0;
}

std::shared_ptr<trevi::SourceBlock> trevi::Decoder::pop()
{
    //    DecodeOutput deco = _output.front();
    //    _output.pop_front();
    //    return deco.block;
    return _buffer->pop();
}
