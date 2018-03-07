
#include "streamdecoder.h"
#include "ogesolver.h"
#include "decoder.h"

// #define USE_LOG

#ifdef USE_PROFILING
#include "profiler.h"
#endif

#include <memory>
#include <iostream>

using namespace std;

trevi::StreamDecoder::StreamDecoder(uint16_t decodingWindowSize)
    :_parent(nullptr), _decodingWindowSize(decodingWindowSize)
{
    _curMinSeqIdx = 0;
    _curMaxSeqIdx = 0;
    _infPacketIdxCount = 0;
    _oge = std::make_shared<OGESolver>( _decodingWindowSize );
    _buffer = std::make_shared<ReorderingBuffer>( _decodingWindowSize );
}

trevi::StreamDecoder::~StreamDecoder()
{

}

void trevi::StreamDecoder::addCodeBlock(uint8_t *buffer, int bufferSize)
{

}

void trevi::StreamDecoder::addCodeBlock(std::shared_ptr<trevi::CodeBlock> cb)
{

#ifdef USE_PROFILING
    rmt_ScopedCPUSample(StreamDecoder_addCodeBlock, 0);
#endif

    std::set< uint32_t > compo = cb->getCompositionSet();

#ifdef USE_LOG
    //    cerr << "decoder:" << endl;
    //    cerr << "decoder_seq_idx=" << cb->get_stream_sequence_idx() << endl;
    //    cb->dumpCompositionBitset();
    //    cb->dumpCompositionField();
    //    print_bytes( cerr, "decoder_input", cb->buffer_ptr(), cb->buffer_size() );
    //    cerr << "__________________" << endl;
#endif

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
        if( _infPacketIdxCount > 2 )
        {
            // sleep(1);
            _curMinSeqIdx = 0;
            _curMaxSeqIdx = 0;
            _oge->reset();
            if( _parent )
            {
                _parent->clear();
            }
            _infPacketIdxCount = 0;
            cerr << "reset." << endl;
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
        trevi::DecodeOutput doutput = _oge->_output.front();
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

bool trevi::StreamDecoder::available()
{
    return _buffer->available();
}

std::shared_ptr<trevi::SourceBlock> trevi::StreamDecoder::pop()
{
    return _buffer->pop();
}

void trevi::StreamDecoder::setParent(trevi::Decoder *parent)
{
    _parent = parent;
}
