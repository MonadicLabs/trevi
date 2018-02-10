
#include "xor.h"
#include "../include/utils.h"

#define USE_GF256

#ifdef USE_GF256
#ifdef __arm__
#define LINUX_ARM
#define HAVE_ARM_NEON_H
#endif
#include "gf256.h"
#else
#include "utils.h"
#endif

void trevi_simd_xor( uint8_t* buffer_a, uint8_t* buffer_b, unsigned int bufferSize )
{
/*
extern "C" void gf256_add_mem(void * GF256_RESTRICT vx,
                              const void * GF256_RESTRICT vy, int bytes)
{
    GF256_M128 * GF256_RESTRICT x16 = reinterpret_cast<GF256_M128 *>(vx);
    const GF256_M128 * GF256_RESTRICT y16 = reinterpret_cast<const GF256_M128 *>(vy);

#if defined(GF256_TARGET_MOBILE)
# if defined(GF256_TRY_NEON)
    // Handle multiples of 64 bytes
    if (CpuHasNeon)
    {
        while (bytes >= 64)
        {
            GF256_M128 x0 = vld1q_u8((uint8_t*) x16);
            GF256_M128 x1 = vld1q_u8((uint8_t*)(x16 + 1) );
            GF256_M128 x2 = vld1q_u8((uint8_t*)(x16 + 2) );
            GF256_M128 x3 = vld1q_u8((uint8_t*)(x16 + 3) );
            GF256_M128 y0 = vld1q_u8((uint8_t*)y16);
            GF256_M128 y1 = vld1q_u8((uint8_t*)(y16 + 1));
            GF256_M128 y2 = vld1q_u8((uint8_t*)(y16 + 2));
            GF256_M128 y3 = vld1q_u8((uint8_t*)(y16 + 3));

            vst1q_u8((uint8_t*)x16,     veorq_u8(x0, y0));
            vst1q_u8((uint8_t*)(x16 + 1), veorq_u8(x1, y1));
            vst1q_u8((uint8_t*)(x16 + 2), veorq_u8(x2, y2));
            vst1q_u8((uint8_t*)(x16 + 3), veorq_u8(x3, y3));

            bytes -= 64, x16 += 4, y16 += 4;
        }

        // Handle multiples of 16 bytes
        while (bytes >= 16)
        {
            GF256_M128 x0 = vld1q_u8((uint8_t*)x16);
            GF256_M128 y0 = vld1q_u8((uint8_t*)y16);

            vst1q_u8((uint8_t*)x16, veorq_u8(x0, y0));

            bytes -= 16, ++x16, ++y16;
        }
    }
    else
# endif // GF256_TRY_NEON
    {
        uint64_t * GF256_RESTRICT x8 = reinterpret_cast<uint64_t *>(x16);
        const uint64_t * GF256_RESTRICT y8 = reinterpret_cast<const uint64_t *>(y16);

        const unsigned count = (unsigned)bytes / 8;
        for (unsigned ii = 0; ii < count; ++ii)
            x8[ii] ^= y8[ii];

        x16 = reinterpret_cast<GF256_M128 *>(x8 + count);
        y16 = reinterpret_cast<const GF256_M128 *>(y8 + count);

        bytes -= (count * 8);
    }
#else // GF256_TARGET_MOBILE
# if defined(GF256_TRY_AVX2)
    if (CpuHasAVX2)
    {
        GF256_M256 * GF256_RESTRICT x32 = reinterpret_cast<GF256_M256 *>(x16);
        const GF256_M256 * GF256_RESTRICT y32 = reinterpret_cast<const GF256_M256 *>(y16);

        while (bytes >= 128)
        {
            GF256_M256 x0 = _mm256_loadu_si256(x32);
            GF256_M256 y0 = _mm256_loadu_si256(y32);
            x0 = _mm256_xor_si256(x0, y0);
            GF256_M256 x1 = _mm256_loadu_si256(x32 + 1);
            GF256_M256 y1 = _mm256_loadu_si256(y32 + 1);
            x1 = _mm256_xor_si256(x1, y1);
            GF256_M256 x2 = _mm256_loadu_si256(x32 + 2);
            GF256_M256 y2 = _mm256_loadu_si256(y32 + 2);
            x2 = _mm256_xor_si256(x2, y2);
            GF256_M256 x3 = _mm256_loadu_si256(x32 + 3);
            GF256_M256 y3 = _mm256_loadu_si256(y32 + 3);
            x3 = _mm256_xor_si256(x3, y3);

            _mm256_storeu_si256(x32, x0);
            _mm256_storeu_si256(x32 + 1, x1);
            _mm256_storeu_si256(x32 + 2, x2);
            _mm256_storeu_si256(x32 + 3, x3);

            bytes -= 128, x32 += 4, y32 += 4;
        }

        // Handle multiples of 32 bytes
        while (bytes >= 32)
        {
            // x[i] = x[i] xor y[i]
            _mm256_storeu_si256(x32,
                _mm256_xor_si256(
                    _mm256_loadu_si256(x32),
                    _mm256_loadu_si256(y32)));

            bytes -= 32, ++x32, ++y32;
        }

        x16 = reinterpret_cast<GF256_M128 *>(x32);
        y16 = reinterpret_cast<const GF256_M128 *>(y32);
    }
    else
# endif // GF256_TRY_AVX2
    {
        while (bytes >= 64)
        {
            GF256_M128 x0 = _mm_loadu_si128(x16);
            GF256_M128 y0 = _mm_loadu_si128(y16);
            x0 = _mm_xor_si128(x0, y0);
            GF256_M128 x1 = _mm_loadu_si128(x16 + 1);
            GF256_M128 y1 = _mm_loadu_si128(y16 + 1);
            x1 = _mm_xor_si128(x1, y1);
            GF256_M128 x2 = _mm_loadu_si128(x16 + 2);
            GF256_M128 y2 = _mm_loadu_si128(y16 + 2);
            x2 = _mm_xor_si128(x2, y2);
            GF256_M128 x3 = _mm_loadu_si128(x16 + 3);
            GF256_M128 y3 = _mm_loadu_si128(y16 + 3);
            x3 = _mm_xor_si128(x3, y3);

            _mm_storeu_si128(x16, x0);
            _mm_storeu_si128(x16 + 1, x1);
            _mm_storeu_si128(x16 + 2, x2);
            _mm_storeu_si128(x16 + 3, x3);

            bytes -= 64, x16 += 4, y16 += 4;
        }
    }
#endif // GF256_TARGET_MOBILE

#if !defined(GF256_TARGET_MOBILE)
    // Handle multiples of 16 bytes
    while (bytes >= 16)
    {
        // x[i] = x[i] xor y[i]
        _mm_storeu_si128(x16,
            _mm_xor_si128(
                _mm_loadu_si128(x16),
                _mm_loadu_si128(y16)));

        bytes -= 16, ++x16, ++y16;
    }
#endif

    uint8_t * GF256_RESTRICT x1 = reinterpret_cast<uint8_t *>(x16);
    const uint8_t * GF256_RESTRICT y1 = reinterpret_cast<const uint8_t *>(y16);

    // Handle a block of 8 bytes
    const int eight = bytes & 8;
    if (eight)
    {
        uint64_t * GF256_RESTRICT x8 = reinterpret_cast<uint64_t *>(x1);
        const uint64_t * GF256_RESTRICT y8 = reinterpret_cast<const uint64_t *>(y1);
        *x8 ^= *y8;
    }

    // Handle a block of 4 bytes
    const int four = bytes & 4;
    if (four)
    {
        uint32_t * GF256_RESTRICT x4 = reinterpret_cast<uint32_t *>(x1 + eight);
        const uint32_t * GF256_RESTRICT y4 = reinterpret_cast<const uint32_t *>(y1 + eight);
        *x4 ^= *y4;
    }

    // Handle final bytes
    const int offset = eight + four;
    switch (bytes & 3)
    {
    case 3: x1[offset + 2] ^= y1[offset + 2];
    case 2: x1[offset + 1] ^= y1[offset + 1];
    case 1: x1[offset] ^= y1[offset];
    default:
        break;
    }
}
*/
}

void trevi_xor(uint8_t *buffer_a, uint8_t *buffer_b, unsigned int bufferSize)
{
#ifdef USE_GF256
    gf256_add_mem( buffer_a, buffer_b, bufferSize );
#else
    dumbXOR( buffer_a, buffer_b, bufferSize );
#endif
}


void init_xor()
{
#ifdef USE_GF256
    gf256_init();
#endif
}
