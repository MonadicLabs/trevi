
#include "streamencoder.h"
#include "profiler.h"
#include "xor.h"

#include <memory>
#include <iostream>
#include <random>
#include <set>
#include <chrono>
#include <iomanip>

#include <cstring>

using namespace std;

trevi::StreamEncoder::StreamEncoder(int encodingWindowSize, int numSourceBlockPerCodeBlock, int numCodeBlockPerSourceBlock)
    :_encodingWindowSize(encodingWindowSize), _numSourceBlockPerCodeBlock(numSourceBlockPerCodeBlock), _numCodeBlockPerSourceBlock(numCodeBlockPerSourceBlock)
{
    init();
}

trevi::StreamEncoder::~StreamEncoder()
{

}

void trevi::StreamEncoder::addData(std::shared_ptr<trevi::SourceBlock> cb, uint8_t streamId )
{

#ifdef USE_PROFILING
    $
        #endif

            // Add to encoding buffer
            cb->updateCRC();
    pushSourceBlock(cb);

    if( _encodingWindow.size() > 0 && _curSeqIdx % _numSourceBlockPerCodeBlock == 0 )
    {
        for( int j = 0; j < _numCodeBlockPerSourceBlock; ++j )
        {
            auto polbak = createEncodedBlock( pickDegree() );
            if( polbak != nullptr )
            {
                polbak->set_stream_id(streamId);
                _codeBlocks.push_back( polbak );
            }
        }
    }
    // dumpCodeBlocks();
    // cerr << "_curSeqIdx=" << _curSeqIdx << endl;
}

void trevi::StreamEncoder::addData(uint8_t *buffer, int bufferSize)
{
    std::shared_ptr< trevi::SourceBlock > cb = std::make_shared<trevi::SourceBlock>( buffer, bufferSize );
    addData( cb );
}

void trevi::StreamEncoder::dumpEncodingWindow()
{
    for( int i = 0; i < _encodingWindow.size(); ++i )
    {
        cerr << _encodingWindow[i] << " ";
    }
    cerr << endl;
}

bool trevi::StreamEncoder::hasEncodedBlocks()
{
    return _codeBlocks.size() > 0;
}

std::shared_ptr<trevi::CodeBlock> trevi::StreamEncoder::getEncodedBlock()
{
    std::shared_ptr< trevi::CodeBlock > ret = nullptr;
    if( hasEncodedBlocks() )
    {
        ret = _codeBlocks.front();
        _codeBlocks.pop_front();
    }
    return ret;
}

void trevi::StreamEncoder::init()
{
    _curSeqIdx = 0;
    srand( time(NULL) );
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    degree_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void trevi::StreamEncoder::pushSourceBlock(std::shared_ptr<trevi::SourceBlock> cb, uint8_t streamId )
{
    if( _encodingWindow.size() >= _encodingWindowSize )
    {
        _encodingWindow.pop_front();
        _sourceBlockBuffer.pop_front();
    }
    _encodingWindow.push_back( _curSeqIdx );
    _sourceBlockBuffer.push_back( cb );

    // Also output the degree 1 block immediatly
    std::shared_ptr< trevi::CodeBlock > d1cb = std::make_shared< trevi::CodeBlock >( cb->buffer_size(), (uint8_t*)cb->buffer_ptr(), cb->buffer_size() );
    std::set<uint32_t> compoSet;
    compoSet.insert(0);
    d1cb->setCompositionField(_curSeqIdx, compoSet );
    d1cb->set_stream_id(streamId);
    _codeBlocks.push_back( d1cb );

    _curSeqIdx++;
}

uint32_t trevi::StreamEncoder::oldestSeqIdx()
{
    return _encodingWindow.front();
}

uint32_t trevi::StreamEncoder::latestSeqIdx()
{
    return _encodingWindow.back();
}

std::shared_ptr<trevi::CodeBlock> trevi::StreamEncoder::createEncodedBlock(uint16_t degree)
{

    std::set<uint32_t> compoSet;
    std::uniform_int_distribution<int> distribution(0,_encodingWindow.size()-1);
    while( compoSet.size() != degree )
    {
        // Select a source packet at random
        int number = distribution(generator);
        int seqNum = _encodingWindow[ number ];
        if( compoSet.find( number ) == compoSet.end() )
        {
            compoSet.insert( number );
        }
    }

    // Compute MTU for current set of blocks
    int maxBlockSize = 0;
    std::set<uint32_t>::iterator it;
    for (it=compoSet.begin(); it!=compoSet.end(); ++it)
    {
        int idx = *it;
        std::shared_ptr< trevi::SourceBlock > curCb = _sourceBlockBuffer[ idx ];
        if( curCb->buffer_size() > maxBlockSize )
        {
            maxBlockSize = curCb->buffer_size();
        }
    }

    uint8_t tmpBuffer_A[ maxBlockSize ];
    uint8_t tmpBuffer_B[ maxBlockSize ];
    memset( tmpBuffer_A, 0, maxBlockSize );

    //    std::cout << "set:" << endl;
    //    printSet( compoSet );
    //    std::cout << "....." << endl;

    // std::cout << "set:" << endl;
    for (it=compoSet.begin(); it!=compoSet.end(); ++it)
    {
        int idx = *it;
        std::shared_ptr< trevi::SourceBlock > curCb = _sourceBlockBuffer[ idx ];
        memset( tmpBuffer_B, 0, maxBlockSize );
        trevi_memcpy( tmpBuffer_B, curCb->buffer_ptr(), curCb->buffer_size() );
        trevi_xor( tmpBuffer_A, tmpBuffer_B, maxBlockSize );
    }

    std::shared_ptr< trevi::CodeBlock > cbcode = std::make_shared<trevi::CodeBlock>( maxBlockSize, (uint8_t*)tmpBuffer_A, (int)maxBlockSize );
    cbcode->setCompositionField( oldestSeqIdx(), compoSet );

    return cbcode;

}

std::shared_ptr<trevi::CodeBlock> trevi::StreamEncoder::selectRandomSourceBlock()
{
    return nullptr;
}

uint16_t trevi::StreamEncoder::pickDegree()
{
    std::uniform_int_distribution<int> distribution(1, min((int)_encodingWindowSize, (int)_encodingWindow.size()) );
    uint16_t ret = distribution(degree_generator);
    return ret;
}

void trevi::StreamEncoder::dumpCodeBlocks()
{
    cerr << "###### CURRENT COEBLOCKS:" << endl;
    for( std::shared_ptr< trevi::CodeBlock > cb : _codeBlocks )
    {
        cb->dumpCompositionField();
    }
    cerr << "#########################" << endl;
}

void trevi::StreamEncoder::setCodeRate(int nsrc, int ncode)
{
    if( nsrc > 0 )
        _numSourceBlockPerCodeBlock = nsrc;

    if( ncode >= 0 )
        _numCodeBlockPerSourceBlock = ncode;
}
