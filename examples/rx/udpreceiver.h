#pragma once

#include <string>

class UDPReceiver
{
public:
    UDPReceiver(uint16_t port, const std::string& networkInterface="", const std::string& multicastGroup="" , int timeout = 1000 );
    virtual ~UDPReceiver();

    virtual int receive(uint8_t *buffer, uint32_t bufferSize);

private:
    int _timeout;
    int _fd;
    uint16_t _port;
    std::string _address;
    std::string _networkInterface;

protected:

};
