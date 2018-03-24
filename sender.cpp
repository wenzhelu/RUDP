//
//  sender.cpp
//  ReliableUDP
//
//  Created by Wenzhe Lu on 2018/3/22.
//  Copyright © 2018年 Wenzhe Lu. All rights reserved.
//

#include "include/RUDP.hpp"
#include "include/sender.hpp"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <chrono>   // for timing

//Sender::Sender(char *buffer, size_t len) {
//    send(buffer, len);
//}
//
//Sender::Sender(char *file) {
//    send(file);
//}

Sender::Sender() : userBuff(nullptr), userDataLen(0), close(false), resend(false), pending(false), diff(0) {}

// note that we only support sending file with length in unsigned int range
void Sender::send(char *file) {
    busy.lock();
    std::fstream fs(file);
    struct stat st;
    stat(file, &st);
    userBuff = new char[st.st_size];
    size_t pt = 0;
    
    while (fs.good()) {
        fs.read(userBuff + pt, 4096);
        pt += fs.gcount();
    }
    
    if (!fs.eof())
        std::cout << "read file error" << std::endl;
    
    userDataLen = st.st_size;
    pending = true;
    diff = RUDP::sendBase;
    curPtr = 0;
}

void Sender::send(char *buffer, size_t len) {
    busy.lock();
    userBuff = new char[len];
    memcpy(userBuff, buffer, len);
    userDataLen = len;
    pending = true;
    diff = RUDP::sendBase;
    curPtr = 0;
}

// the sending thread will loop in this function
void Sender::sending() {
    while (!close) {
        this_thread::sleep_for(chrono::milliseconds(RUDP::RTT / 2)); // sleep time need to test
        // we are making somehow dangerous assumption here.
        // the listener may need to change the curPtr when
        // packet loss happened
        if (pending) {
            // data pending to send, copy data to the buff
            if (RUDP::sendBase - diff == userDataLen) {
                // all buffer received
                // no data pending to send
                // unlock busy
                pending = false;
                delete [] userBuff;
                userBuff = nullptr;
                userDataLen = 0;
                diff = 0;
                busy.unlock();
                goto NoData;
            }
            
            if (resend) {
                // packet loss, request resend by listener
                curPtr = RUDP::sendBase - diff;
                resend = false;
            }
            
            char *srcBegin = userBuff + curPtr, *destBegin = RUDP::buff + RUDP::HEADER_LEN;
            uint len = std::min((uint)1460, std::min(RUDP::cWnd, (uint)(userDataLen - curPtr)));
            memcpy(destBegin, srcBegin, len);
            
            // finally we are sending
            // socket send here
            
            // change the sequence number after sending packet out
            uint *tm = (uint*)RUDP::buff;
            *tm += len;
            curPtr += len;
        } else {
        NoData:
            RUDP::setDataBit(0);
            
            // if no pending data, check if there is pending ACK
            // if no then do nothing
            // if yes just send it
            if (RUDP::testAckBit()) {
                // TODO: send packet out, must send before reset
                RUDP::setAckBit(false);
                
            }
        }
    }
}

