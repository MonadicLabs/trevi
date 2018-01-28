#pragma once

#include <string>

#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

bool isMulticastAddress(const std::string &ipAddress);

std::string getNetworkInterfaceIP(const std::string &interfaceName);

