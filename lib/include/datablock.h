#pragma once

#include <memory>

namespace trevi
{
class DataBlock
{
public:
    DataBlock();
    virtual ~DataBlock();

    void * buffer_ptr();
    int buffer_size();

    virtual uint16_t payload_size(){ return 0; }
    virtual void * payload_ptr(){ return nullptr; }

private:

protected:
    void initMemory( int bufferSize );
    int                     _bufferSize;
    std::shared_ptr<void>   _data;

};
}
