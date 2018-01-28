


# Trevi

### An implementation of a streaming RLNC forward error correction code, inspired by:
> [Wunderlich, S., Gabriel, F., Pandi, S., Fitzek, F. H., & Reisslein, M. (2017). Caterpillar RLNC (CRLNC): A Practical Finite Sliding Window RLNC Approach. IEEE Access, 5, 20183-20197.](http://ieeexplore.ieee.org/iel7/6287639/7859429/08052109.pdf)

This early implementation is very crude, but results should be the same as stated in the paper.

The code is not really optimized, processing time on my old i7 based laptop are of this order of magnitude (encoding window size=32, decoding window size=64) :
| Operation | processing time (usec) |
|--|--|
| Encoding | 152.311 |
| Decoding | 53.6886 |

### Implementation principle
TODO

### Current implementation limitations
Encoding window size is limited to 32 (coded packet composition are encoded as a bitfield on a uint32) before being sent to the decoder. Decoding window size can be arbitrarily large, but must be strictly superior to the encoding window size (33 if encoding window size is 32, for example).

## Getting Started

### Prerequisites
This is a C++ project. Only tested on Linux for now.
You will need CMake to compile it properly. Also you will need an SSE3 capable CPU to run it (not sure if mandatory).

### Compiling
Using CMake out of source compilation:

(in trevi source directory)

    mkdir build
    cd build
    cmake ..
    make
   
   There are also several compilation options, to enable/disable profiling, etc. Just check using ccmake or other cmake UI tools.

## Example applications
There is no real API documentation for trevi (apart from the comments in trevi.h, for now !) but several example applications are provided

### UDP loss recovery
Two projects are provided, aimed at simulation UDP packet loss and recovery using trevi. I use them to test reliability of x264/RTP streaming over a lossy UDP channel.
#### trevi_tx
This application grabs packets from an UDP port, encodes them and writes the output packets to another UDP port. Check the "usage" option of this application to get more details:

    usage: ./examples/tx/trevi_tx [options] ... 
    options:
      -i, --input_port      UDP port for input data (int [=5000])
      -o, --output_port     UDP port for encoded output data (int [=5001])
      -h, --output_host     address of destination host for encoded data (string [=127.0.0.1])
      -e, --window_size     Encoding window size (must be inferior or equal to 32) (int [=32])
      -s, --nsrc_blocks     Number of source block after which we send code blocks (int [=1])
      -c, --ncode_blocks    Number of coded blocks to send after processing nsrc_blocks (int [=1])
      -p, --loss_proba      Simulated random uniform packet loss probability (float [=0])
      -?, --help            print this message
     
As you can see, if you want to simulate a lossy channel, this is where you specify the loss parameter.

#### trevi_rx
This application grabs packets from an UDP port, decodes them and writes the output decoded packets to another UDP port. Check the "usage" option of this application to get more details:

    usage: ./examples/rx/trevi_rx [options] ... 
    options:
      -i, --input_port     UDP port for encoded input data (int [=5001])
      -o, --output_port    UDP port for decoded output data (int [=5002])
      -h, --output_host    address of destination host for decoded data (string [=127.0.0.1])
      -d, --window_size    Decoding window size (must be strictly superior to encoding window size) (int [=64])
      -?, --help           print this message

#### Testing Trevi for reliable x264/RTP video streaming
You can quickly test Trevi for reliable video streaming over lossy channels using both trevi_tx and trevi_rx, along with a simple gstreamer setup.
##### Setting Trevi encoder and decoder
First, launch **trevi_tx**:

    ./examples/tx/trevi_tx -p 0.1
  This will launch the encoder listening on port 5000 for input data, and outputing encoded data on port 5001 (localhost), simulating a uniform packet loss of probability 0.1 (10% loss):

    nicolas@xxxx:~/dev/trevi/build$ ./examples/tx/trevi_tx -p 0.1
    Starting Trevi UDP encoder: 
    UDP input port for input data: 			5000
    UDP output for encoded data: 			127.0.0.1:5001
    Simulated channel loss probability: 			0.1

Then, also launch the decoder, **trevi_rx**:

    ./examples/rx/trevi_rx
This will launch the decoder, with default parameters:

    nicolas@xxxx:~/dev/trevi/build$ ./examples/rx/trevi_rx 
    Starting Trevi UDP decoder: 
    UDP input port for encoded data: 			5001
    UDP output for decoded data: 			127.0.0.1:5002

##### Setting gstreamer UDP streaming and output visualization
First, launch the x264 streaming, for example:

    gst-launch-1.0 v4l2src ! x264enc bitrate=3000 tune=zerolatency ! rtph264pay pt=96 mtu=512 ! udpsink host=127.0.0.1 port=5000
This will start streaming the webcam on port 5000 (localhost), using an h264 encoder (bitrate 3Mbit/s). MTU size is not important, but for debug purposes, I usually use 512 or 1024 bytes.

Then, start the visualization gstreamer pipeline:

    gst-launch-1.0 udpsrc port=5002 caps ='application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264' ! rtph264depay ! decodebin ! autovideosink sync=false
This gstreamer pipeline will display the Trevi-decoded video grabbed from port 5002, in a graphical window.

### Benchmark application
There is also a benchmark application that should be able to replicate several results of the original paper.
You can test various parameters, such as:

 - Encoding window size
 - Decoding window size
 - Simulated Gilbert-Elliot loss model parameters

Just launch **trevi_bench** to test various parameters. If everything goes well, you should be able to reproduce the results from the paper:

    ./examples/bench/trevi_bench

Usage:

    usage: ./examples/bench/trevi_bench [options] ... 
    options:
      -e, --encoding_window_size     Encoding window size (must be inferior or equal to 32) (int [=32])
      -d, --decoding_window_size     Decoding window size (must be strictly superior to encoding window size) (int [=64])
      -s, --nsrc_blocks              Number of source block after which we send code blocks (int [=1])
      -c, --ncode_blocks             Number of coded blocks to send after processing nsrc_blocks (int [=1])
      -b, --bad_state_proba          Simulated loss - steady state probability for the bad state of Gilbert-Elliot model (float [=0.05])
      -B, --expected_burst_length    Simulated loss - expected sojourn time in the bad state (ie. error burst length) (float [=4])
      -?, --help                     print this message

Example output:

    current source packets processed: 100000
    Average decode processing time = 131.886 microsec.
    Average encode processing time = 45.6679 microsec.
    Channel packet loss: 20371 / 100000 Channel loss probability: 20.371%
    Packets unrecovered: 209 / 100000 - Packet loss probability: 0.209%
    (Average delay - decoding_window_size) (in slots): 1.47159


### Credits
Christopher Taylor ([catid)](https://github.com/catid/) for his [Galois Field arithmetic implementation](https://github.com/catid/gf256)
Logging provided by [loguru](https://github.com/emilk/loguru)
Awesome profiling using [dollar](https://github.com/r-lyeh-archived/dollar)



