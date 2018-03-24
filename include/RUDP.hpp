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
    static const short PACKET_SIZE = 1460;  // max packet size in byte
    static const short HEADER_LEN = 12;     // packet header length
    
    static uint sendBase;       // send base
    static uint winSize;        // Note that this parameter is set by administrator
    static uint throughput;     // set when first timeout happend, defaultly INT_MAX
    static statusEnum status;   //
    static uint cWnd;           // Congestion Window
    static uint RTT;            // updated by listener
    static map<uint, milliseconds> startTimes;
    static char buff[1472];     // 1500 - 8(UDP) - 20(IP)
    static Sender sender;
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
    static void init(uint window) {
        srand(time(nullptr));   // set random seed
        sendBase = rand() % 1000;
        winSize = window;
        status = SLOW_START;
        cWnd = PACKET_SIZE;
//        milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
    
    static void close() {
        
    }
};

#endif /* RUDP_hpp */
