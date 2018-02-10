#pragma once

#include <memory>

#include <cstdint>

#include "sourceblock.h"

namespace trevi {
typedef struct
{
    std::shared_ptr< trevi::SourceBlock > block;
    uint32_t stream_idx;
    uint32_t global_idx;
} DecodeOutput;

struct DecodeOutputComparator
{
    inline bool operator() (const DecodeOutput& struct1, const DecodeOutput& struct2)
    {
        return (struct1.global_idx < struct2.global_idx);
    }
};
}
