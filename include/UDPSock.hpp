//
//  UDPSock.hpp
//  ReliableUDP
//

#ifndef UDPSock_hpp
#define UDPSock_hpp

#include <cstddef>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// a helper class for UDP socket
class Usock {
private:
    int socket_fd;
    struct sockaddr_in local;
    struct sockaddr_in remote;
    // can add socket options later

public:
    Usock();
    
    int init(const char *localPort, const char *remoteIp, const char *remotePort);
    
    // proxy functions
    size_t read(char[], size_t);
    
    size_t write(char[], size_t);
    
    void printPacket(bool, char*, uint);
    
    void closeSock();
    
    ~Usock();
};

#endif /* UDPSock_hpp */
