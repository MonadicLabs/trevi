#include <iostream>
#include <random>
#include <chrono>

#include <trevi.h>

#include "udptransmitter.h"
#include "udpreceiver.h"
#include "Timer.h"
#include "cmdline.h"

int main( int argc, char** argv )
{

    cmdline::parser a;

    a.add<int>("input_port", 'i', "UDP port for input data", false, 5000 );
    a.add<int>("output_port", 'o', "UDP port for encoded output data", false, 5001 );
    a.add<string>("output_host", 'h', "address of destination host for encoded data", false, "127.0.0.1" );
    a.add<int>("window_size", 'e', "Encoding window size (must be inferior or equal to 32)", false, 32 );
    a.add<int>("nsrc_blocks", 's', "Number of source block after which we send code blocks", false, 1 );
    a.add<int>("ncode_blocks", 'c', "Number of coded blocks to send after processing nsrc_blocks", false, 1 );
    a.add<float>("loss_proba", 'p', "Simulated random uniform packet loss probability", false, 0.0f, cmdline::range(0.0, 1.0) );

    a.parse_check(argc, argv);

    int udpInputPort = a.get<int>("input_port");
    int udpOutputPort = a.get<int>("output_port");
    std::string udpOutputHost = a.get<string>("output_host");
    int encodingWindowSize = a.get<int>("window_size");
    int nSrcBlocks = a.get<int>("nsrc_blocks");
    int nCodeBlocks = a.get<int>("ncode_blocks");

    float packetLossProba = a.get<float>("loss_proba");
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());

    cerr << "Starting Trevi UDP encoder: " << endl;
    cerr << "UDP input port for input data: \t\t\t" << udpInputPort << endl;
    cerr << "UDP output for encoded data: \t\t\t" << udpOutputHost << ":" << udpOutputPort << endl;
    cerr << "Simulated channel loss probability: \t\t\t" << packetLossProba << endl;
    sleep(2);

    trevi_init();

    // Create a Trevi encoder
    trevi_encoder * encoder = trevi_create_encoder();

    // Add a new stream to the encoder, here stream of id 0, with an encoding window size of 32
    trevi_encoder_add_stream( encoder, 0, encodingWindowSize, nSrcBlocks, nCodeBlocks );

    UDPReceiver udpr( udpInputPort );
    UDPTransmitter udpt( udpOutputPort, udpOutputHost  );

    uint8_t buffer[ 2048 ];

    double t_sum = 0.0;
    int iterCpt = 0;
    Timer t;

    t.start();
    for(;;)
    {
        int esize = 0;
        int rsize = udpr.receive( buffer, 2048 );
        if( rsize > 0)
        {
            Timer t;
            t.start();
            trevi_encode( encoder, 0, buffer, rsize );
            t.stop();
            t_sum += t.getElapsedTimeInMicroSec();

            while(true)
            {
                esize = trevi_encoder_get_encoded_data( encoder, buffer );
                if( esize > 0 )
                {
                    std::uniform_real_distribution<float> distribution(0.0, 1.0);
                    float p = distribution(generator);
                    if( p > packetLossProba )
                    {
                        udpt.send( buffer, esize );
                    }
                    else
                    {
                        // LOSS !
                    }
                }
                else
                {
                    break;
                }
            }
        }

        iterCpt++;

        if( iterCpt % 1000 == 0 )
        {
            double t_encode = t_sum / (double)iterCpt;
            cerr << "Average Encode processing time = " << t_encode << " microsec." << endl;
        }
    }

    return 0;
}
