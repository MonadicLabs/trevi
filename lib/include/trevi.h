#pragma once

#include "block.h"
#include "codeblock.h"
#include "streamencoder.h"
#include "streamdecoder.h"
#include "encoder.h"
#include "decoder.h"

#include "profiler.h"

typedef struct
{
    // Reference to our encoder object
    void * encoderRef;

    // References to stream encoders
    // We can encoder up to 32 different streams with different encoding parameters
    void * streamEncoderRefs[ 32 ];

} trevi_encoder;

typedef struct
{
    void * decoderRef;
    void * streamDecoderRefs[ 32 ];
} trevi_decoder;

///
/// \brief trevi_init
/// Initialize Trevi.
void trevi_init();

///
/// \brief trevi_create_encoder
/// \return a pointer to a trevi_encoder struct
/// Creates a new encoder to use. You still need to add at least one stream to begin encoding
/// You can add up to 32 different streams with different encoding parameters
/// The decoder will detect which packet belongs to which stream and will merge and reorder the decoded packets automatically
trevi_encoder * trevi_create_encoder();

///
/// \brief trevi_encoder_add_stream Add a new stream to the encoder
/// \param encoder Pointer to a trevi_encoder object
/// \param streamId Identifier of the new stream to create
/// \param encodingWindowSize Size of the encoding window. Must be greater than 1, obviously, and less or equal to 32 (for implementation reasons)
/// \param num_source_block_per_code_block Number of source packets after which we send the encoded packets
/// \param num_code_block_per_source_block Number of encoded packets to send after num_source_block_per_code_block packets
/// \return 0 if succesful, negative error code otherwise
///
int trevi_encoder_add_stream( trevi_encoder* encoder, int streamId, int encodingWindowSize, int num_source_block_per_code_block = 1, int num_code_block_per_source_block = 1);

///
/// \brief trevi_encoder_remove_stream
/// \param encoder Pointer to the encoder from which we would like to remove the stream encoder
/// \param streamId Identifier of the stream
/// \return 0 if succesful, negative error code otherwise
///
int trevi_encoder_remove_stream( trevi_encoder* encoder, int streamId );

///
/// \brief trevi_encode Encode a new packet
/// \param encoder Pointer to the encoder
/// \param streamId Identifier of the stream that should process this packet
/// \param buffer Pointer to the data that will be processed
/// \param bufferSize Size of the data buffer
/// \return 0 if succesful, negative error code otherwise
///
int trevi_encode( trevi_encoder* encoder, int streamId, const void* buffer, int bufferSize );

///
/// \brief trevi_encoder_get_encoded_data Return encoded data
/// \param encoder Pointer to the encoder
/// \param out_buffer Pointer to the buffer to which output data will be written
/// \return -1 if no encoded blocks are available, size of the written data otherwise
///
int trevi_encoder_get_encoded_data( trevi_encoder* encoder, const void* out_buffer );

///
/// \brief trevi_encoder_set_stream_coderate
/// \param streamId
/// \param num_source_block_per_code_block
/// \param num_code_block_per_source_block
/// \return
///
int trevi_encoder_set_stream_coderate(trevi_encoder *encoder, int streamId, int num_source_block_per_code_block, int num_code_block_per_source_block );

///
/// \brief trevi_create_decoder Create a new decoder
/// \return A pointer to a newly created trevi_decoder
///
trevi_decoder * trevi_create_decoder();

///
/// \brief trevi_decoder_add_stream Declare new stream for decoding
/// You must declare as much streams as you did in your encoder, or you will lose packets.
/// \param decoder Pointer to the decoder
/// \param streamId Identifier of the stream (must be the same as declared in your encoder)
/// \param decodingWindowSize Decoding window size for this stream. Must be at least greater than the encoding window size of the encoding stream
/// \return 0 if succesful, negative error code otherwise
///
int trevi_decoder_add_stream( trevi_decoder* decoder, int streamId, int decodingWindowSize );

///
/// \brief trevi_decoder_remove_stream Remove a stream from a decoder
/// \param decoder Pointer to the decoder
/// \param streamId Identifier of the stream decoder to remove from the decoder
/// \return 0 if succesful, negative error code otherwise
///
int trevi_decoder_remove_stream( trevi_decoder* decoder, int streamId );

///
/// \brief trevi_decode Process coded packets
/// \param decoder Pointer to the decoder
/// \param buffer pointer to the encoded data that will be processed
/// \param bufferSize Size of the data (in bytes) stored in buffer
/// \return 0 if succesful, negative error code otherwise
///
int trevi_decode( trevi_decoder* decoder, const void* buffer, int bufferSize );

///
/// \brief trevi_decoder_get_decoded_data Retrieve decoded data from the decoder
/// \param decoder Pointer to the decoder
/// \param out_buffer Buffer of bytes to which decoded data will be written
/// \param packetSeqIdx Pointer to an unsigned int, value will be modified to contain the sequence_number of the retrieve packet (so you can track unrecovered packets -ie. definitly lost-)
/// \return Number of bytes written to out_buffer
///
int trevi_decoder_get_decoded_data( trevi_decoder* decoder, const void* out_buffer, unsigned int* packetSeqIdx );
