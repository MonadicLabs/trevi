
#include "udputils.h"

bool isMulticastAddress(const std::string &ipAddress)
{
    in_addr_t a = inet_addr( ipAddress.c_str() );
    //in_addr_t stored in network order
    uint32_t address = ntohl(a);
    return (address & 0xF0000000) == 0xE0000000;
}


std::string getNetworkInterfaceIP(const std::string &interfaceName)
{
    std::string ret = "";
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if((strcmp(ifa->ifa_name, interfaceName.c_str())==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                exit(EXIT_FAILURE);
            }
            // printf("\tInterface : <%s>\n",ifa->ifa_name );
            // printf("\t  Address : <%s>\n", host);
            ret = host;
        }
    }
    freeifaddrs(ifaddr);
    return ret;
}
