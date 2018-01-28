#pragma once

#include <string>
#include <cstdint>

class UDPTransmitter
{
public:
    UDPTransmitter( uint16_t port, std::string host, std::string multicastInterface="" );
    virtual ~UDPTransmitter();

    virtual void send(uint8_t *data, uint32_t data_len);

private:
    int _fd;
    uint16_t _port;
    std::string _host;
    std::string _multicastInterface;

protected:

};
