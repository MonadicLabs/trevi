#include <iostream>
#include <trevi.h>

#include <cstring>

#include "Timer.h"
#include "gilbertelliot.h"
#include "cmdline.h"

using namespace std;

int generateRandomSourceBlock( uint8_t* buffer, int blockSize )
{
    memset( (void*)buffer, 0, blockSize );
    for( int k = 0; k < blockSize; ++k )
    {
        buffer[k] = (uint8_t)(rand()%256);
    }
}

int main( int argc, char** argv )
{

    cmdline::parser a;

    a.add<int>("encoding_window_size", 'e', "Encoding window size (must be inferior or equal to 32)", false, 32 );
    a.add<int>("decoding_window_size", 'd', "Decoding window size (must be strictly superior to encoding window size)", false, 64 );

    a.add<int>("nsrc_blocks", 's', "Number of source block after which we send code blocks", false, 1 );
    a.add<int>("ncode_blocks", 'c', "Number of coded blocks to send after processing nsrc_blocks", false, 1 );

    a.add<float>("bad_state_proba", 'b', "Simulated loss - steady state probability for the bad state of Gilbert-Elliot model", false, 0.2f, cmdline::range(0.0, 1.0) );
    a.add<float>("expected_burst_length", 'B', "Simulated loss - expected sojourn time in the bad state (ie. error burst length)", false, 4.0f );

    a.parse_check(argc, argv);

    int encodingWindowSize = a.get<int>( "encoding_window_size" );
    int decodingWindowSize = a.get<int>( "decoding_window_size" );
    int nSrcBlocks = a.get<int>( "nsrc_blocks" );
    int nCodeBlocks = a.get<int>( "ncode_blocks" );

    float badStateProba = a.get<float>( "bad_state_proba" );
    float burstLength = a.get<float>( "expected_burst_length" );

    trevi_init();

    GilbertElliot ge = GilbertElliot::fromBadProbabilityAndBurstLength( badStateProba, burstLength );
    cerr << "Gilbert-Elliot beta=" << ge.beta << " gamma=" << ge.gamma << endl;
    sleep(1);

    uint8_t buffer[ 2048 ];

    trevi_encoder * encoder = trevi_create_encoder();
    trevi_encoder_add_stream( encoder, 0, encodingWindowSize, nSrcBlocks, nCodeBlocks );

    trevi_decoder * decoder = trevi_create_decoder();
    trevi_decoder_add_stream( decoder, 0, decodingWindowSize );

    double t_encode_sum = 0.0;
    double t_decode_sum = 0.0;

    int encodeOpsCpt = 0.0;
    int decodeOpsCpt = 0.0;

    int dataBlockSize = 1024;
    int numBenchmarkIter = 10000000;
    int lossesCpt = 0;              // Total simulated losses
    int encodedPacketsCpt = 0;      // Total encoded packets generated

    int lastPktIdx = -1;

    int packetLossCpt = 0;          // Total packets unrecovered
    int packetRecovCpt = 0;         // Total packets recovered

    double delaySum = 0.0;          // Delay (in slots)

    for(int i = 0; i < numBenchmarkIter; ++i )
    {

        // Generate a random data block to be encoded.
        generateRandomSourceBlock( buffer, dataBlockSize );

        // Pass it to the encoder
        Timer t;
        t.start();
        trevi_encode( encoder, 0, buffer, dataBlockSize );
        t.stop();
        encodeOpsCpt++;
        t_encode_sum += t.getElapsedTimeInMicroSec();

        while(true)
        {
            int esize = trevi_encoder_get_encoded_data( encoder, buffer );
            if( esize > 0 )
            {
                // Simulate loss according to GE model.
                if( ge.sample() )
                {
                    // Pass encoded blocks to the decoder
                    t.start();
                    trevi_decode( decoder, buffer, esize );
                    t.stop();
                    decodeOpsCpt++;
                    t_decode_sum += t.getElapsedTimeInMicroSec();
                }
                else
                {
                    // We lost a packet on the channel
                    lossesCpt++;
                }
                encodedPacketsCpt++;
            }
            else
            {
                break;
            }
        }

        while( true )
        {
            unsigned int pktIdx;
            int dsize = trevi_decoder_get_decoded_data( decoder, buffer, &pktIdx );
            if( dsize <= 0 )
            {
                break;
            }
            else
            {
                packetRecovCpt++;
                delaySum += (double)(i-pktIdx-decodingWindowSize);
                // We have our original data back. Let's do something with it
                if( pktIdx != lastPktIdx + 1)
                {
                    // Unrecovered packet detected.
                    for( int k = 1; k < pktIdx - lastPktIdx; ++k )
                    {
#ifdef VERBOSE_OUTPUT
                        cerr << "# Definitly lost packet seqidx=" << lastPktIdx + k << endl;
#endif
                        packetLossCpt++;
                    }
                }
                lastPktIdx = pktIdx;
#ifdef VERBOSE_OUTPUT
                cerr << "pktIdx=" << pktIdx << endl;
#endif
            }
        }

        if( i % 1000 == 0 )
        {
            cerr << "current source packets processed: " << i << endl;
            double t_decode = t_decode_sum / (double)decodeOpsCpt;
            cerr << "Average decode processing time = " << t_decode << " microsec." << endl;
            double t_encode = t_encode_sum / (double)encodeOpsCpt;
            cerr << "Average encode processing time = " << t_encode << " microsec." << endl;

            double channelPacketLossProbability = (double)lossesCpt / (double)i;
            cerr << "Channel packet loss: " << lossesCpt << " / " << i << " Channel loss probability: " << channelPacketLossProbability * 100.0 << "%" << endl;

            double packetLossProbabilty = (double)packetLossCpt / (double)i;
            cerr << "Packets unrecovered: " << packetLossCpt << " / " << i << " - Packet loss probability: " << packetLossProbabilty * 100.0 << "%" << endl;
            double avgDelay = delaySum / (double)packetRecovCpt;
            cerr << "(Average delay - decoding_window_size) (in slots): " << avgDelay << endl;
            cerr << "-----------------------------------------------------------------------" << endl;
        }

        dump_profiling_info();
        // sleep(1);

    }

    return 0;
}
