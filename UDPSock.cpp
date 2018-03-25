//
//  UDPSock.cpp
//  ReliableUDP
//

#include "include/UDPSock.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <errno.h>

#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif
#define debug_print(fmt, ...) \
do { if (DEBUG_FLAG) { \
fprintf(stderr, "[DEBUG] ");\
fprintf(stderr, fmt, __VA_ARGS__); } \
} while (0)

Usock::Usock() {}

int Usock::init(const char *localPort, const char *remoteIp, const char *remotePort) {
    
    // init of the usock do three things:
    // 1.setup socket
    // 2.bind socket to certain port
    // 3.initialize remote information
    
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        printf("socket setup error\n");
        return -1;
    }
    
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(localPort));
    if (bind(socket_fd, (struct sockaddr *) &local, sizeof(local)) < 0) {
        printf("server binding error\n");
        return -1;
    }
    
    remote.sin_addr.s_addr = inet_addr(remoteIp);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(remotePort));
    
    return 0;
}

size_t Usock::read(char *buff, size_t len) {
    return recvfrom(socket_fd, buff, len, 0, nullptr, 0);
}

size_t Usock::write(char *buff, size_t len) {
    size_t written = sendto(socket_fd, buff, len, 0, (struct sockaddr *)&remote, sizeof(remote));
//    printf("%d\n", errno);
    if (written != len) {
        debug_print("write socket error\n", NULL);
    }
    return written;
}

void Usock::closeSock() {
    close(socket_fd);
}

void Usock::printPacket(bool listen, char* buff, uint bufflen) {
    if (!DEBUG_FLAG) return;    // only print these while debugging

    uint *tm = (uint *)buff;
    
    if (listen) debug_print("==============Receiving packet==============\n", nullptr);
    else debug_print("================Sending packet=============\n", nullptr);
    if (buff[8] & 0x80) {
        debug_print("Sequence number: %u\n", *tm);
    } else {
        debug_print("No data\n", nullptr);
    }
    
    tm++;
    if (buff[8] & 0x40) {
        debug_print("Ack number: %u\n", *tm);
    } else {
        debug_print("No ACK\n", nullptr);
    }
    
    tm += 2;
    if (buff[8] & 0x80) {
        debug_print("data section:\n%.*s\n", bufflen - 12, (char *)tm);
    }
    
    if (listen) debug_print("==============Receiving packet ends==============\n\n", nullptr);
    else debug_print("================Sending packet ends=============\n\n", nullptr);
}

Usock::~Usock() {
    
}

