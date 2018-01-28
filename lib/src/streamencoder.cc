
#include "streamencoder.h"
#include "gf256.h"
#include "profiler.h"

#include <memory>
#include <iostream>
#include <random>
#include <set>
#include <chrono>
#include <iomanip>

#include <cstring>

using namespace std;

StreamEncoder::StreamEncoder(int encodingWindowSize, int numSourceBlockPerCodeBlock, int numCodeBlockPerSourceBlock)
    :_encodingWindowSize(encodingWindowSize), _numSourceBlockPerCodeBlock(numSourceBlockPerCodeBlock), _numCodeBlockPerSourceBlock(numCodeBlockPerSourceBlock)
{
    init();
}

StreamEncoder::~StreamEncoder()
{

}

void StreamEncoder::addData(std::shared_ptr<SourceBlock> cb)
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
                _codeBlocks.push_back( polbak );
            }
        }
    }
}

void StreamEncoder::addData(uint8_t *buffer, int bufferSize)
{
    std::shared_ptr< SourceBlock > cb = std::make_shared<SourceBlock>( buffer, bufferSize );
    addData( cb );
}

void StreamEncoder::dumpEncodingWindow()
{
    for( int i = 0; i < _encodingWindow.size(); ++i )
    {
        cerr << _encodingWindow[i] << " ";
    }
    cerr << endl;
}

bool StreamEncoder::hasEncodedBlocks()
{
    return _codeBlocks.size() > 0;
}

std::shared_ptr<CodeBlock> StreamEncoder::getEncodedBlock()
{
    std::shared_ptr< CodeBlock > ret = nullptr;
    if( hasEncodedBlocks() )
    {
        ret = _codeBlocks.front();
        _codeBlocks.pop_front();
    }
    return ret;
}

void StreamEncoder::init()
{
    _curSeqIdx = 0;
    srand( time(NULL) );
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    degree_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void StreamEncoder::pushSourceBlock(std::shared_ptr<SourceBlock> cb)
{
    if( _encodingWindow.size() >= _encodingWindowSize )
    {
        _encodingWindow.pop_front();
        _sourceBlockBuffer.pop_front();
    }
    _encodingWindow.push_back( _curSeqIdx );
    _sourceBlockBuffer.push_back( cb );

    // Also output the degree 1 block immediatly
    std::shared_ptr< CodeBlock > d1cb = std::make_shared< CodeBlock >( cb->buffer_size(), (uint8_t*)cb->buffer_ptr(), cb->buffer_size() );
    std::set<uint32_t> compoSet;
    compoSet.insert(0);
    d1cb->setCompositionField(_curSeqIdx, compoSet );
    _codeBlocks.push_back( d1cb );

    _curSeqIdx++;
}

uint32_t StreamEncoder::oldestSeqIdx()
{
    return _encodingWindow.front();
}

uint32_t StreamEncoder::latestSeqIdx()
{
    return _encodingWindow.back();
}

std::shared_ptr<CodeBlock> StreamEncoder::createEncodedBlock(uint16_t degree)
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
        std::shared_ptr< SourceBlock > curCb = _sourceBlockBuffer[ idx ];
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
        std::shared_ptr< SourceBlock > curCb = _sourceBlockBuffer[ idx ];
        memset( tmpBuffer_B, 0, maxBlockSize );
        memcpy( tmpBuffer_B, curCb->buffer_ptr(), curCb->buffer_size() );
        gf256_add_mem( tmpBuffer_A, tmpBuffer_B, maxBlockSize );
        // dumbXOR( tmpBuffer_A, tmpBuffer_B, maxBlockSize );
    }

    //    if( degree == 1 )
    //        print_bytes( cerr, "addData_sb", (unsigned char*)tmpBuffer, maxBlockSize, true );

    std::shared_ptr< CodeBlock > cbcode = std::make_shared<CodeBlock>( maxBlockSize, (uint8_t*)tmpBuffer_A, (int)maxBlockSize );
    //    if( degree == 1 )
    //        print_bytes( cerr, "addData_cb", (unsigned char*)cbcode->payload_ptr(), cbcode->payload_size(), true );

    cbcode->setCompositionField( oldestSeqIdx(), compoSet );
    //    cerr << "dump compofield" << endl;
    //    cbcode->dumpCompositionField();
    //    cerr << "......" << endl;
    cbcode->updateCRC();

    return cbcode;

}

std::shared_ptr<CodeBlock> StreamEncoder::selectRandomSourceBlock()
{
    return nullptr;
}

uint16_t StreamEncoder::pickDegree()
{
    std::uniform_int_distribution<int> distribution(1, min((int)_encodingWindowSize, (int)_encodingWindow.size()) );
    return distribution(degree_generator);
}
