#pragma once

#include <map>

#include <cstdint>

#include "streamencoder.h"

namespace trevi {
class Encoder
{
public:
    Encoder();
    virtual ~Encoder();

    void addStream( int stremaId, StreamEncoder* encoder );
    void removeStream( int streamId );

    void addData( int streamId, std::shared_ptr<trevi::SourceBlock> sb );
    void addData( int streamId, uint8_t* buffer, int bufferSize );

    bool hasEncodedBlocks();
    std::shared_ptr< trevi::CodeBlock > getEncodedBlock();

    void setStreamCoderate(int streamId, int nsrc, int ncode );

private:
    std::map< uint8_t, StreamEncoder* > _encoders;
    uint32_t _curGlobalIdx;

protected:

};
}
