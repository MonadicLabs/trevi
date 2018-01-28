
#include "datablock.h"

DataBlock::DataBlock()
{

}

DataBlock::~DataBlock()
{

}

void *DataBlock::buffer_ptr()
{
    return _data.get();
}

int DataBlock::buffer_size()
{
    return _bufferSize;
}

void DataBlock::initMemory(int bufferSize)
{
    _bufferSize = bufferSize;
    char* rawPtr = (char*)malloc( _bufferSize );
    _data = std::shared_ptr<void>( (void*)(rawPtr), free );
}
