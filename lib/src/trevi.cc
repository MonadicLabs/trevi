
#include "trevi.h"
#include "gf256.h"

#include "encoder.h"

#include <iostream>

using namespace std;

#include <unistd.h>

void trevi_init()
{
    int initret = gf256_init();
}


trevi_encoder *trevi_create_encoder()
{
    trevi_encoder * ret = new trevi_encoder;

    ret->encoderRef = (void*)(new Encoder());
    for( int k = 0; k < 32; ++k )
    {
        ret->streamEncoderRefs[k] = nullptr;
    }

    return ret;
}


int trevi_encoder_add_stream(trevi_encoder *encoder, int streamId, int encodingWindowSize, int num_source_block_per_code_block, int num_code_block_per_source_block)
{
    if( streamId >= 32 )
        return -1; // Nope, only up to 32 different streams

    if( encoder->streamEncoderRefs[streamId] != 0 )
        return -1; // Stream encoder already exists

    StreamEncoder * streamEnc = new StreamEncoder(encodingWindowSize, num_source_block_per_code_block, num_code_block_per_source_block);
    encoder->streamEncoderRefs[ streamId ] = (void*)(streamEnc);
    Encoder * enc = reinterpret_cast<Encoder*>(encoder->encoderRef);
    enc->addStream( streamId, streamEnc );

    return 0;
}


int trevi_encode(trevi_encoder *encoder, int streamId, const void *buffer, int bufferSize)
{
    Encoder * enc = reinterpret_cast<Encoder*>(encoder->encoderRef);
    enc->addData( streamId, buffer, bufferSize );
    return 0;
}


int trevi_encoder_get_encoded_data(trevi_encoder *encoder, const void *out_buffer)
{
     Encoder * enc = reinterpret_cast<Encoder*>(encoder->encoderRef);

     if( enc->hasEncodedBlocks() )
     {
         std::shared_ptr< CodeBlock > cb = enc->getEncodedBlock();
         memcpy( out_buffer, cb->buffer_ptr(), cb->buffer_size() );
         return cb->buffer_size();
     }

     return -1;

}


trevi_decoder *trevi_create_decoder()
{
    trevi_decoder * ret = new trevi_decoder;

    ret->decoderRef = (void*)(new Decoder());
    for( int k = 0; k < 32; ++k )
    {
        ret->streamDecoderRefs[k] = nullptr;
    }

    return ret;
}

int trevi_decoder_add_stream(trevi_decoder *decoder, int streamId, int decodingWindowSize)
{
    if( streamId >= 32 )
        return -1; // Nope, only up to 32 different streams

    if( decoder->streamDecoderRefs[streamId] != 0 )
        return -1; // Stream encoder already exists

    StreamDecoder * streamDec = new StreamDecoder(decodingWindowSize);
    decoder->streamDecoderRefs[ streamId ] = (void*)(streamDec);
    Decoder * dec = reinterpret_cast<Decoder*>(decoder->decoderRef);
    dec->addStream( streamId, streamDec );

    return 0;
}


int trevi_decode(trevi_decoder *decoder, const void *buffer, int bufferSize)
{
    Decoder * dec = reinterpret_cast<Decoder*>(decoder->decoderRef);
    dec->addCodeBlock(buffer, bufferSize);
    return 0;
}


int trevi_decoder_get_decoded_data(trevi_decoder *decoder, const void *out_buffer, unsigned int * packetSeqIdx)
{
    Decoder * dec = reinterpret_cast<Decoder*>(decoder->decoderRef);
    if( dec->available() )
    {
        std::shared_ptr< SourceBlock > sb = dec->pop();
        (*packetSeqIdx) = sb->get_global_sequence_idx();
        memcpy( out_buffer, sb->payload_ptr(), sb->payload_size() );
        return sb->payload_size();
    }

    return -1;
}
