
#include "streamdecoder.h"
#include "ogesolver.h"
#include "decoder.h"

#ifdef USE_PROFILING
#include "profiler.h"
#endif

#include <memory>
#include <iostream>

using namespace std;

StreamDecoder::StreamDecoder(uint16_t decodingWindowSize)
    :_parent(nullptr), _decodingWindowSize(decodingWindowSize)
{
    _curMinSeqIdx = 0;
    _curMaxSeqIdx = 0;
    _infPacketIdxCount = 0;
    _oge = std::make_shared<OGESolver>( _decodingWindowSize );
    _buffer = std::make_shared<ReorderingBuffer>( _decodingWindowSize );
}

StreamDecoder::~StreamDecoder()
{

}

void StreamDecoder::addCodeBlock(uint8_t *buffer, int bufferSize)
{

}

void StreamDecoder::addCodeBlock(std::shared_ptr<CodeBlock> cb)
{

#ifdef USE_PROFILING
    $
        #endif

            std::set< uint32_t > compo = cb->getCompositionSet();
    auto minSeqIdx = *compo.begin();
    auto maxSeqIdx = *compo.rbegin();

#ifdef USE_LOG
    cerr << "block_min=" << minSeqIdx << " block_max=" <<maxSeqIdx << endl;
#endif

    // Check if encoder has reset ?
    // cerr << "minSeqIdx=" << minSeqIdx << " - _curMinSeqIdx=" << _curMinSeqIdx << " _infPacketIdxCount=" << _infPacketIdxCount << endl;
    if( minSeqIdx < _curMinSeqIdx )
    {
        _infPacketIdxCount++;
        if( _infPacketIdxCount > _decodingWindowSize )
        {
            sleep(1);
            _curMinSeqIdx = minSeqIdx;
            _curMaxSeqIdx = minSeqIdx;
            _oge->reset();
            _infPacketIdxCount = 0;
        }
    }

    // Resize to the right...
    if( maxSeqIdx > _curMaxSeqIdx )
    {
#ifdef USE_LOG
        cerr << "resize to the right..." << endl;
#endif
        _curMaxSeqIdx = maxSeqIdx + 1;
    }

    // Compute new span if any...
    int span = _curMaxSeqIdx - _curMinSeqIdx;

    // Resize to the left...
    int targetMin = max( (int)0, (int)(_curMaxSeqIdx - _decodingWindowSize + 1)  );
    _curMinSeqIdx = targetMin;

    // cerr << "_curMaxSeqIdx=" << _curMaxSeqIdx << " _curMinSeqIdx=" << _curMinSeqIdx << endl;

    if( _curMinSeqIdx > _oge->offset() )
    {
        _oge->shiftWindowTo( _curMinSeqIdx );
    }

    // cerr << std::dec << "New min/max values: " << _curMinSeqIdx << " - " << _curMaxSeqIdx << endl;

    _oge->addBlock( cb );

#ifdef USE_LOG
    cerr << "Number of undetermined blocks: " << _oge->undeterminedCount();
#endif

    while( _oge->_output.size() > 0 )
    {
        DecodeOutput doutput = _oge->_output.front();
        _oge->_output.pop_front();
        if( _parent )
        {
#ifdef USE_LOG
            cerr << "Calling onNewDecodeOutput !" << endl;
#endif
            _parent->onNewDecodeOutput(doutput);
        }
    }

    // _oge->dump();

}

bool StreamDecoder::available()
{
    return _buffer->available();
}

std::shared_ptr<SourceBlock> StreamDecoder::pop()
{
    return _buffer->pop();
}

void StreamDecoder::setParent(Decoder *parent)
{
    _parent = parent;
}
