#include <iostream>
#include <trevi.h>

#include "udpreceiver.h"
#include "udptransmitter.h"
#include "Timer.h"
#include "cmdline.h"

using namespace std;

int main( int argc, char** argv )
{
    cmdline::parser a;

    a.add<int>("input_port", 'i', "UDP port for encoded input data", false, 5001 );
    a.add<string>("input_mcast_addr", 'm', "Multicast address for encoded input data", false, "");
    a.add<string>("mcast_iface", 'I', "Multicast interface for encoded input data", false, "lo");
    a.add<int>("output_port", 'o', "UDP port for decoded output data", false, 5002 );
    a.add<string>("output_host", 'h', "address of destination host for decoded data", false, "127.0.0.1" );
    a.add<int>("window_size", 'd', "Decoding window size (must be strictly superior to encoding window size)", false, 64 );

    a.parse_check(argc, argv);

    int udpInputPort = a.get<int>("input_port");
    int udpOutputPort = a.get<int>("output_port");
    string udpOutputHost = a.get<string>("output_host");
    string multicastAddr = a.get<string>("input_mcast_addr");
    string multicastIface = "";
    if( multicastAddr.size() > 0 )
    {
        multicastIface = a.get<string>("mcast_iface");
    }
    cerr << "Starting Trevi UDP decoder: " << endl;
    cerr << "UDP input port for encoded data: \t\t\t" << udpInputPort << endl;
    cerr << "UDP output for decoded data: \t\t\t" << udpOutputHost << ":" << udpOutputPort << endl;
    sleep(2);

    trevi_init();

    UDPReceiver udpr( udpInputPort, multicastIface, multicastAddr );
    UDPTransmitter udpt( udpOutputPort, udpOutputHost );

    int dsize = 0;
    unsigned int pktIdx;

    uint8_t buffer[ 2048 ];

    trevi_decoder * decoder = trevi_create_decoder();
    trevi_decoder_add_stream( decoder, 0, a.get<int>("window_size") );

    double t_sum = 0.0;
    int iterCpt = 0;

    for(;;)
    {
        int rsize = udpr.receive( buffer, 2048 );
        if( rsize > 0)
        {
            // cerr << "rx size=" << rsize << endl;
            Timer t;
            t.start();
            trevi_decode( decoder, buffer, rsize );
            t.stop();
            t_sum += t.getElapsedTimeInMicroSec();

            while( true )
            {
                dsize = trevi_decoder_get_decoded_data( decoder, buffer, &pktIdx );
                if( dsize < 0 )
                {
                    break;
                }
                else
                {
                    // cerr << "dsize=" << dsize << " pktIdx=" << pktIdx << endl;
                    udpt.send( buffer, dsize );
                }
            }
        }

        iterCpt++;

        if( iterCpt % 1000 == 0 )
        {
            double t_decode = t_sum / (double)iterCpt;
            cerr << "Average decode processing time = " << t_decode << " microsec." << endl;
        }

        dump_profiling_info();

    }

    return 0;
}
