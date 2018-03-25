//
//  sender.cpp
//  ReliableUDP
//

#include "include/RUDP.hpp"
#include "include/sender.hpp"
#include "include/UDPSock.hpp"
//#include "include/TimeoutTimer.hpp"

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include <thread>
#include <chrono>   // for timing

Sender::Sender(RUDP *r) : userBuff(nullptr), userDataLen(0), resend(false), pending(false), diff(0), master(r), packets(0) {}

// note that we only support sending file with length in unsigned int range
void Sender::send(const char *file) {
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
    diff = master->sendBase;
    curPtr = master->sendBase;
//    fs.close();
}

void Sender::send(const char *buffer, size_t len) {
    busy.lock();
    userBuff = new char[len];
    memcpy(userBuff, buffer, len);
    userDataLen = len;
    pending = true;
    diff = master->sendBase;
    curPtr = master->sendBase;
}

// the sending thread will loop in this function
void Sender::sending() {
    while (!master->close) {
        this_thread::sleep_for(chrono::milliseconds(master->RTT / 2)); // sleep time need to test
        // we are making somehow dangerous assumption here.
        // the listener may need to change the curPtr when
        // packet loss happened
        if (pending) {
            if (master->sendBase - diff >= userDataLen) {
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
                curPtr = master->sendBase;
                resend = false;
            }
            
            char *srcBegin = userBuff + (curPtr - diff), *destBegin = master->buff + master->HEADER_LEN;
            uint len = std::min((uint)1460, std::min(byteIncWnd(), userDataLen - (curPtr - diff)));
            
            // len == 0 could be: we have sent all the data but
            // waiting for all the ack
            // or we have reached send out cWnd of bytes and waiting for ACK to
            // "slide" the cWnd
            if (len > 0) {
                master->setDataBit(1);
                memcpy(destBegin, srcBegin, len);
                
                // finally we are sending
                master->sock->printPacket(false, master->buff, master->HEADER_LEN + len);
                master->startTimes.insert(pair<uint, tp>(curPtr + len, system_clock::now()));
                master->sock->write(master->buff, master->HEADER_LEN + len);
                if (packets++ % 3 == 0) {
                    // setup timer
//                    l.push_back(new TimeoutTimer(ms(master->TimeOut), this, curPtr));
                }
                master->setAckBit(0);   // set ack 0
                // if we have any sending error, the usock class
                // will print debug infomation, and we just ignore any
                // sending error here, we let ACK and resend to fix it.
                
                // when timer times up, we need to know whether we have
                // also, we need this to calculate the RTT
                
                // set up timer
//                TimeoutTimer(milliseconds(master->TimeOut), this, curPtr);
                
                // change the sequence number after sending packet out
                curPtr += len;
                uint *tm = (uint*)master->buff;
                *tm = curPtr;
                continue;
            }
        }
        master->setDataBit(0);
            
        // if no pending data, check if there is pending ACK
        // if no then do nothing
        // if yes just send it
        if (master->testAckBit()) {
            master->sock->printPacket(false, master->buff, master->HEADER_LEN);
            master->sock->write(master->buff, master->HEADER_LEN);
            if (packets++ % 3 == 0) {
                // setup timer
//                l.push_back(new TimeoutTimer(ms(master->TimeOut), this, curPtr));
            }
            
            master->setAckBit(0);
        }
    }
}

uint Sender::byteIncWnd() {
    return master->cWnd - (curPtr - master->sendBase);
}

