
#include "reorderingbuffer.h"

#include <iostream>
#include <algorithm>

using namespace std;

#include <unistd.h>

void ReorderingBuffer::addBlock(uint32_t stream_idx, uint32_t global_idx, std::shared_ptr<trevi::SourceBlock> sb)
{
    // If the idx is far enough, reset buffer and start clean...

    // If the current buffer size is less than max window size, then we can insert anyway...
    // Drop blocks until ok...
    while( _buffer.size() >= _windowSize )
    {
        _outputQueue.push_back( _buffer.front().block );
        _buffer.pop_front();
    }

    insert( stream_idx, global_idx, sb );

#ifdef USE_LOG
    dump();
#endif


}

bool ReorderingBuffer::available()
{
    return _outputQueue.size() > 0;
}

std::shared_ptr<trevi::SourceBlock> ReorderingBuffer::pop()
{
    std::shared_ptr<trevi::SourceBlock> ret = nullptr;
    if( _outputQueue.size() > 0 )
    {
        ret = _outputQueue.front();
        _outputQueue.pop_front();
    }
    return ret;
}

void ReorderingBuffer::insert(uint32_t stream_idx, uint32_t global_idx, std::shared_ptr<trevi::SourceBlock> sb)
{
    trevi::DecodeOutput deco;
    deco.block = sb;
    deco.stream_idx = stream_idx;
    deco.global_idx = global_idx;
    _buffer.push_back( deco );
    std::sort( _buffer.begin(), _buffer.end(), trevi::DecodeOutputComparator() );
}

void ReorderingBuffer::dump()
{
    cerr << "~~~~ REORDERING BUFFER:" << endl;
    for( int i = 0; i < _buffer.size(); ++i )
    {
        cerr << "[" << _buffer[i].global_idx << "] ";
    }
    cerr << endl;
    // usleep(100000);
}
