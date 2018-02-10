
#include "../include/datablock.h"
#include "../include/utils.h"

trevi::DataBlock::DataBlock()
{

}

trevi::DataBlock::~DataBlock()
{

}

void *trevi::DataBlock::buffer_ptr()
{
    return _data.get();
}

int trevi::DataBlock::buffer_size()
{
    return _bufferSize;
}

void trevi::DataBlock::initMemory(int bufferSize)
{
    _bufferSize = bufferSize;
    char* rawPtr = (char*)trevi_malloc( _bufferSize );
    _data = std::shared_ptr<void>( (void*)(rawPtr), free );
}
