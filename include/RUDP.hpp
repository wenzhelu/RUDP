//
//  RUDP.hpp
//  ReliableUDP
//
//  Created by Wenzhe Lu on 2018/3/22.
//  Copyright © 2018年 Wenzhe Lu. All rights reserved.
//

#ifndef RUDP_hpp
#define RUDP_hpp

typedef unsigned int uint;

#include <cstdlib>
#include <chrono>
#include <map>
#include "sender.hpp"
#include "UDPSock.hpp"

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

using namespace std;
using namespace chrono;    // for milliseconds

typedef enum status {
    SLOW_START,
    FAST_REC,
    CONG_AVOID,
} statusEnum;

// class for the protocol
class RUDP {
    
private:
    RUDP() {}
    
public:
    static const short PACKET_SIZE = 1460;  // max packet size in byte - only data here
    static const short HEADER_LEN = 12;     // packet header length
    
    static uint sendBase;       // send base
    static uint winSize;        // Note that this parameter is set by administrator
    static uint throughput;     // set when first timeout happend, defaultly INT_MAX
    static statusEnum status;   //
    static uint cWnd;           // Congestion Window
    static uint RTT;            // updated by listener
    static map<uint, milliseconds> startTimes;
    static char buff[PACKET_SIZE + HEADER_LEN];     // 1500 - 8(UDP) - 20(IP)
    static Sender sender;
    static bool close;
    static Usock sock;
//    static uint timeout;      // we decide to let timer to set RTT
    
    static void setDataBit(bool tf){
        if(tf)
            buff[8] |= 1 << 7;
        else
            buff[8] &= ~(1 << 7);
    }
    
    static void setAckBit(bool tf){
        if(tf)
            buff[8] |= 1 << 6;
        else
            buff[8] &= ~(1 << 6);
    }
    
    static bool testDataBit() {
        return (buff[8] >> 7) & 1;
    }
    static bool testAckBit() {
        return (buff[8] >> 6) & 1;
    }
    
    // in charge of setting default parameters and
    // intializing sending and listener
    static void init(uint window, const char *port, const char *remoteIp, const char *remotePort) {
        srand(time(nullptr));   // set random seed
        sendBase = rand() % 1000;
        // make the sequence number in the buffer the same with send base
        *(uint *)(buff) = sendBase;
        winSize = window;
        status = SLOW_START;
        cWnd = PACKET_SIZE;
        close = false;
        sock.init(port, remoteIp, remotePort);
        // TODO: spawn threads to run listener and sender
        
//        milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
    
    static void shutdown() {
        // stop protocol running
        close = true;
        // TODO: join sender and listener thread
    }
    
    static void cleanUp() {
        sock.closeSock();
        // release resources like socket and heap memory
    }
};

#endif /* RUDP_hpp */
