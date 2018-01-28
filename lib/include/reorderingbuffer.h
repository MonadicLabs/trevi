#pragma once

#include <deque>
#include <vector>

#include <cstdint>

#include "sourceblock.h"
#include "decodeoutput.h"

class ReorderingBuffer
{
public:
    ReorderingBuffer( int windowSize )
        :_windowSize(windowSize)
    {

    }

    virtual ~ReorderingBuffer()
    {

    }

    void addBlock( uint32_t stream_idx, uint32_t global_idx, std::shared_ptr< SourceBlock > sb );

    bool available();
    std::shared_ptr< SourceBlock > pop();

private:
    int _windowSize;

    std::deque< DecodeOutput > _buffer;

    std::deque< std::shared_ptr< SourceBlock > > _outputQueue;

    void insert(uint32_t stream_idx, uint32_t global_idx, std::shared_ptr< SourceBlock > sb );

    void dump();

protected:

};
