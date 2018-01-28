#pragma once

#include "codeblock.h"
#include "sourceblock.h"

#include <memory>
#include <deque>
#include <random>

#include <cstdint>

class Encoder;
class StreamEncoder
{

    friend class Encoder;

public:
    StreamEncoder(int encodingWindowSize = 8, int numSourceBlockPerCodeBlock = 1, int numCodeBlockPerSourceBlock = 1);
    virtual ~StreamEncoder();

    void addData(std::shared_ptr<SourceBlock> cb );
    void addData( uint8_t* buffer, int bufferSize );

    void dumpEncodingWindow();

    bool hasEncodedBlocks();
    std::shared_ptr< CodeBlock > getEncodedBlock();

private:
    void init();
    void destroy();

    uint32_t _curSeqIdx;
    uint32_t _encodingWindowSize;

    int _numSourceBlockPerCodeBlock;
    int _numCodeBlockPerSourceBlock;

    std::deque< uint32_t > _encodingWindow;
    std::deque< std::shared_ptr< SourceBlock > > _sourceBlockBuffer;

    void pushSourceBlock( std::shared_ptr< SourceBlock > cb );
    uint32_t oldestSeqIdx();
    uint32_t latestSeqIdx();

    std::default_random_engine generator;
    std::default_random_engine degree_generator;

    std::shared_ptr< CodeBlock > createEncodedBlock( uint16_t degree );
    std::shared_ptr< CodeBlock > selectRandomSourceBlock();
    uint16_t pickDegree();

    std::deque< std::shared_ptr< CodeBlock > > _codeBlocks;

    Encoder * _parent;

protected:

};
