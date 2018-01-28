
#include "encoder.h"


Encoder::Encoder()
    :_curGlobalIdx(0)
{

}

Encoder::~Encoder()
{

}

void Encoder::addStream(int streamId, StreamEncoder *encoder)
{
    _encoders.insert( std::make_pair( streamId, encoder ) );
    return;
}

void Encoder::removeStream(int streamId)
{
    auto kv = _encoders.find( streamId );
    if( kv != _encoders.end() )
    {
        _encoders.erase( kv );
    }
}

void Encoder::addData(int streamId, std::shared_ptr<SourceBlock> sb)
{
    // Find the right encoder...
    if( _encoders.find( streamId ) != _encoders.end() )
    {
        StreamEncoder * se = _encoders[ streamId ];
        sb->set_global_sequence_idx(_curGlobalIdx++);
        se->addData(sb);
    }
}

void Encoder::addData(int streamId, uint8_t *buffer, int bufferSize)
{
    std::shared_ptr< SourceBlock > sb = std::make_shared<SourceBlock>(buffer, bufferSize);
    return addData( streamId, sb );
}

bool Encoder::hasEncodedBlocks()
{
    bool ret = false;
    for( auto kv : _encoders )
    {
        if( kv.second->hasEncodedBlocks() )
        {
            ret = true;
            break;
        }
    }
    return ret;
}

std::shared_ptr<CodeBlock> Encoder::getEncodedBlock()
{
    for( auto kv : _encoders )
    {
        if( kv.second->hasEncodedBlocks() )
        {
            return kv.second->getEncodedBlock();
        }
    }
    return nullptr;
}
