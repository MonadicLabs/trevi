
#include "encoder.h"

#include <iostream>

using namespace std;

trevi::Encoder::Encoder()
    :_curGlobalIdx(0)
{

}

trevi::Encoder::~Encoder()
{

}

void trevi::Encoder::addStream(int streamId, StreamEncoder *encoder)
{
    _encoders.insert( std::make_pair( streamId, encoder ) );
    return;
}

void trevi::Encoder::removeStream(int streamId)
{
    auto kv = _encoders.find( streamId );
    if( kv != _encoders.end() )
    {
        _encoders.erase( kv );
    }
}

void trevi::Encoder::addData(int streamId, std::shared_ptr<trevi::SourceBlock> sb)
{
    // Find the right encoder...
    if( _encoders.find( streamId ) != _encoders.end() )
    {
        StreamEncoder * se = _encoders[ streamId ];
        sb->set_global_sequence_idx(_curGlobalIdx++);
        se->addData(sb, streamId);
        // se->dumpCodeBlocks();
    }
}

void trevi::Encoder::addData(int streamId, uint8_t *buffer, int bufferSize)
{
    std::shared_ptr< trevi::SourceBlock > sb = std::make_shared<trevi::SourceBlock>(buffer, bufferSize);
    return addData( streamId, sb );
}

bool trevi::Encoder::hasEncodedBlocks()
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

std::shared_ptr<trevi::CodeBlock> trevi::Encoder::getEncodedBlock()
{
    for( auto kv : _encoders )
    {
        if( kv.second->hasEncodedBlocks() )
        {
            std::shared_ptr<trevi::CodeBlock> ret = kv.second->getEncodedBlock();
            return ret;

        }
    }
    return nullptr;
}


void trevi::Encoder::setStreamCoderate(int streamId, int nsrc, int ncode)
{
    if( _encoders.find( streamId ) != _encoders.end() )
    {
        trevi::StreamEncoder * se = _encoders[ streamId ];
        se->setCodeRate( nsrc, ncode );
    }
}
