
//#include <iostream>
//#include <fstream>

//#include <trevi.h>

//#include "gf256.h"
//#include "udpreceiver.h"
//#include "udptransmitter.h"

//#include <unistd.h>

//#include <cstring>

//using namespace std;

//#ifdef USE_PROFILING
//#include <dollar.hpp>
//#endif

//#define DEBUG_ORDER
//#define TEST_VARIABLE_LENGTH
//#define USE_FEC

//int main( int argc, char** argv )
//{

//    int curIter = 1;

//#ifdef DEBUG_ORDER
//    ofstream ofs( "/tmp/order.txt" );
//#endif

//    srand(time(NULL));

//    int bsize = 2048;
//    uint8_t * popo = new uint8_t[ bsize ];

//    print_bytes( cerr, "originalblock", (const unsigned char*)popo, bsize );

//    StreamEncoder * s0enc = new StreamEncoder(16);
//    Encoder enc;
//    enc.addStream( 0, s0enc );

//    StreamDecoder * s0dec = new StreamDecoder(32);
//    Decoder deco;
//    deco.addStream(0, s0dec);

//    gf256_init();

//    UDPReceiver udpr( 5000 );
//    UDPTransmitter udpt( 6000, "127.0.0.1" );

//    int lossProba = 5;

//    for( ;; )
//    {

//        int rsize = udpr.receive( popo, 2048 );

//        if( rsize <= 0 )
//            continue;

//#ifdef USE_PROFILING
//        {
//            $
//        #endif
//                    enc.addData( 0, popo, rsize );

//#ifdef USE_PROFILING
//        }
//#endif

//#ifdef USE_PROFILING
//        std::ofstream ofsc("/tmp/chrome.json");
//#endif

//#ifdef USE_FEC
//        std::shared_ptr< CodeBlock > cb = nullptr;
//        while( enc.hasEncodedBlocks() )
//        {

//            cb = enc.getEncodedBlock();
//            // print_bytes( cerr, "codeblock", (const unsigned char*)cb->buffer_ptr(), cb->buffer_size() );
//            //            if( cb->degree() == 1 )
//            //            {
//            //                cerr << "cb_payload_size=" << cb->payload_size() << endl;
//            //                std::shared_ptr< SourceBlock > sb = std::make_shared<SourceBlock>( cb->payload_size(), (uint8_t*)cb->payload_ptr() );
//            //                print_bytes( cerr, "sourceblock", (const unsigned char*)sb->payload_ptr(), sb->payload_size() );
//            //            }
//            if( rand() % 100 <= lossProba )
//            {
//#ifdef DEBUG_OUTPUT
//            cerr << "LOSS !" << endl;
//#endif
//            }
//            else
//            {
//#ifdef DEBUG_OUTPUT
//                cerr << "*** NEW INPUT: " << std::dec << endl;
//                cb->dumpCompositionField();
//                // cerr << cb->isCorrectCRC() << endl;
//                cerr << "***************" << endl;
//#endif

//#ifdef USE_PROFILING
//                {
//                    $
//        #endif
//                            deco.addCodeBlock( cb );
//#ifdef USE_PROFILING
//                }
//#endif
//                // usleep(3000);
//            }

//            while( deco.available() )
//            {
//                std::shared_ptr< SourceBlock > sbout = deco.pop();
//                if( sbout )
//                {
//                    int plSize = sbout->payload_size();
//#ifdef DEBUG_OUTPUT
//                    cerr << "&&&&&& GOT A BLOCK ! size=" << plSize << endl;
//#endif
//                    unsigned char * polop = (unsigned char*)(sbout->payload_ptr());
//                    unsigned char val = polop[0];
//#ifdef DEBUG_OUTPUT
//                    cerr << "&&&&&& VAL=" << (int)val << endl;
//#endif
//                    udpt.send( polop, plSize );
//#ifdef DEBUG_ORDER
//                    ofs << (int)val << endl;
//#endif
//                }
//            }
//#else
//        if( rand() % 100 <= lossProba )
//        {
//#ifdef DEBUG_OUTPUT
//            cerr << "LOSS !" << endl;
//#endif
//        }
//        else
//        {
//            udpt.send( popo, rsize );
//        }
//#endif

//#ifdef USE_PROFILING
//        if( curIter % 1000 == 0 )
//        {
//            dollar::text(std::cout);                        // report stats to std::cout in text format; see also csv(), tsv() and markdown()
//            dollar::chrome(ofsc);                           // write tracing results to a json file (for chrome://tracing embedded profiler)
//            dollar::clear();
//            //                exit(0);
//        }
//#endif

//#ifdef USE_FEC
//    }
//#endif

//    curIter++;

//}

//return 0;
//}
