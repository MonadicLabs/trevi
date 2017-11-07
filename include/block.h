#pragma once

#include <vector>
#include <memory>

#include <cstdint>

namespace trevi
{
    class Block
    {
    public:

        Block(){}

        Block( const Block& other )
        {
            *this = other;
        }

        Block& operator = ( const Block& other )
        {
            _data = other._data;
        }

        virtual ~Block(){}

        std::vector< uint64_t > getMembership();
        void setMembership( const std::vector< uint64_t >& values );

    private:
        std::shared_ptr< uint8_t > _data;

    protected:


    };
}
