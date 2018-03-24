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

Sender::Sender() : userBuff(nullptr), userDataLen(0), resend(false), pending(false), diff(0) {}

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
    curPtr = RUDP::sendBase;
    fs.close();
}

void Sender::send(char *buffer, size_t len) {
    busy.lock();
    userBuff = new char[len];
    memcpy(userBuff, buffer, len);
    userDataLen = len;
    pending = true;
    diff = RUDP::sendBase;
    curPtr = RUDP::sendBase;
}

// the sending thread will loop in this function
void Sender::sending() {
    while (!RUDP::close) {
        this_thread::sleep_for(chrono::milliseconds(RUDP::RTT / 2)); // sleep time need to test
        // we are making somehow dangerous assumption here.
        // the listener may need to change the curPtr when
        // packet loss happened
        if (pending) {
            if (RUDP::sendBase - diff >= userDataLen) {
                // all buffer received
                // no data pending to send
                // unlock busy
                pending = false;
                delete [] userBuff;
                userBuff = nullptr;
                userDataLen = 0;
                diff = 0;
                busy.unlock();
                continue;
            }
            
            // data pending to send, copy data to the buff
            if (resend) {
                // packet loss, request resend by listener
                curPtr = RUDP::sendBase;
                resend = false;
            }
            
            char *srcBegin = userBuff + (curPtr - diff), *destBegin = RUDP::buff + RUDP::HEADER_LEN;
            uint len = std::min((uint)1460, std::min(byteIncWnd(), userDataLen - (curPtr - diff)));
            
            // len == 0 could be: we have sent all the data but
            // waiting for all the ack
            // or we have reached send out cWnd of bytes and waiting for ACK to
            // "slide" the cWnd
            if (len > 0) {
                RUDP::setDataBit(1);
                memcpy(destBegin, srcBegin, len);
                
                // finally we are sending
                RUDP::sock.write(RUDP::buff, RUDP::HEADER_LEN + len);
                // if we have any sending error, the usock class
                // will print debug infomation, and we just ignore any
                // sending error here, we let ACK and resend to fix it.
                
                // change the sequence number after sending packet out
                curPtr += len;
                uint *tm = (uint*)RUDP::buff;
                *tm = curPtr;
            }
        } else {
            RUDP::setDataBit(0);
            
            // if no pending data, check if there is pending ACK
            // if no then do nothing
            // if yes just send it
            if (RUDP::testAckBit()) {
                RUDP::sock.write(RUDP::buff, RUDP::HEADER_LEN);
                RUDP::setAckBit(0);
                
            }
        }
    }
}

uint Sender::byteIncWnd() {
    return RUDP::cWnd - (curPtr - RUDP::sendBase);
}

