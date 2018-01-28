
#include "udpreceiver.h"
#include "udputils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <poll.h>

#include <unistd.h>
#include <cstring>

#include <iostream>

using namespace std;

#undef USE_LOGGING

UDPReceiver::UDPReceiver( uint16_t port, const std::string& networkInterface, const std::string& multicastGroup, int timeout )
    :_address(multicastGroup), _port(port), _networkInterface(networkInterface), _fd(-1), _timeout(timeout)
{
    if (( _fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    {
#ifdef USE_LOGGING
        LOG_S(ERROR) << "Error opening datagram socket.";
#endif
    }

    {
        int reuse = 1;
        if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Setting SO_REUSEADDR...ERROR.";
#endif
            close(_fd);
        }
        else
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Setting SO_REUSEADDR...OK.";
#endif
        }

        /* Bind to the proper port number with the IP address */
        /* specified as INADDR_ANY. */
        struct sockaddr_in localSock;
        memset((char *) &localSock, 0, sizeof(localSock));
        localSock.sin_family = AF_INET;
        localSock.sin_port = htons(_port);
        localSock.sin_addr.s_addr = INADDR_ANY;
        if(bind(_fd, (struct sockaddr*)&localSock, sizeof(localSock)))
        {
#ifdef USE_LOGGING
            LOG_S(ERROR) << "Binding datagram socket ERROR";
#endif
            close(_fd);
        }
        else
        {
#ifdef USE_LOGGING
            LOG_S(INFO) << "Binding datagram socket...OK.";
#endif
        }

        // Joining multicast group if needed...
        if( isMulticastAddress( _address ) && _address.size() )
        {
            struct ip_mreq group;
            group.imr_multiaddr.s_addr = inet_addr( _address.c_str() );
            if( _networkInterface.size() > 0 )
            {
                group.imr_interface.s_addr = inet_addr( getNetworkInterfaceIP(_networkInterface).c_str() );
#ifdef USE_LOGGING
                LOG_S(INFO) << "Using interface " << _networkInterface;
#endif
            }
            else
            {
                group.imr_interface.s_addr = inet_addr( "0.0.0.0" );
#ifdef USE_LOGGING
                LOG_S(INFO) << "Using interface " << "ANY";
#endif
            }

            if(setsockopt( _fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
            {
#ifdef USE_LOGGING
                LOG_S(ERROR) << "ERROR adding multicast group";
#endif
                close(_fd);
            }
            else
            {
#ifdef USE_LOGGING
                LOG_S(INFO) << "Adding multicast group " << _address << " ...OK.";
#endif
            }
        }
        else
        {

        }
    }

}

UDPReceiver::~UDPReceiver()
{
    cerr << "UDPReceiver::~UDPReceiver()" << endl;
    if( _fd >= 0 )
        close(_fd);
}

int UDPReceiver::receive(uint8_t *buffer, uint32_t bufferSize)
{
    /*
    char buf[ 1400 ];
    struct sockaddr_in si_other;
    int slen=sizeof(si_other);

    int rlen = recv( _fd, buf, 1400, 0 );
    if( rlen > 0 )
    {
        memcpy( buffer, buf, rlen );
        return rlen;
    }
    return -1;
    */

    char buf[ 2048 ];

    struct pollfd fd;
    int ret;

    fd.fd = _fd; // your socket handler
    fd.events = POLLIN;
    ret = poll(&fd, 1, _timeout); // 1 second for timeout
    switch (ret) {
        case -1:
            // Error
            break;
        case 0:
            // Timeout
            break;
        default:
            int rlen = recv( _fd, buf, 2048, 0 );
            if( rlen > 0 )
            {
                memcpy( buffer, buf, rlen );
                return rlen;
            }
            break;
    }
    return -1;
}
