//
//  sender.cpp
//  ReliableUDP
//

#include "include/RUDP.hpp"
#include "include/sender.hpp"
#include "include/UDPSock.hpp"

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include <thread>
#include <chrono>   // for timing

Sender::Sender(RUDP *r) : userBuff(nullptr), userDataLen(0), resend(false), pending(false), diff(0), master(r), timerBase(-1) {}

uint Sender::getDurMs(const tp& begin, const tp& end) {
    return (end - begin).count() / 1000000;
}

void Sender::beginTask() {
    diff = master->sendBase;
    curPtr = master->sendBase;
    timerBase = master->sendBase;
    sendTime = system_clock::now();
    th_timer = new thread(&Sender::timing, this);
    pending = true;
}

void Sender::endTask() {
    // all buffer received
    // no data pending to send
    // unlock busy
    pending = false;
    delete [] userBuff;
    userBuff = nullptr;
    userDataLen = 0;
    diff = 0;
    master->startTimes.clear();
    th_timer->join();   // must wait for thread ends before deleting
    delete th_timer;
    // sending finish signal
    master->setDataBit(0);
    master->setFinBit(1);
    master->sock->write(master->buff, RUDP::PACKET_SIZE);
    master->setFinBit(0);
    busy.unlock();
    debug_print("send one file end\n", nullptr);
    debug_print("RTT: %u\n, slow start packets: %u, congestion avoidance packets: %u\n", master->RTT, master->slowStartNum, master->congestNum);
}

void Sender::timing() {
    // timer loop in this function
    while (pending) {
        tp cur = system_clock::now();
        if (getDurMs(sendTime, cur) >= master->TimeOut) {
            // time to check sendBase
            // aggressive mode:
            // set up a begin send time before sending the file
            // then every send time later on is set by timer.
            // we continuously check timerBase and sendBase every RTT
            // if sendBase larger than sendBase, we have a time out.
            // Why called aggressive mode? we make timer very active and
            // not rely on "send" behavior to avoid stuck in sending.
//            debug_print("timerBase: %u, sendBase: %u\n", timerBase, master->sendBase);
            if (timerBase >= master->sendBase) {
                // data loss timeout!
                master->status = SLOW_START;
                master->throughput = master->cWnd >> 1;
                master->cWnd = RUDP::PACKET_SIZE;
                debug_print("Timeout! Send base: %u, cWnd: %u, throughput: %u\n", master->sendBase, master->cWnd, master->throughput);
                resend = true;
            } else {
                timerBase = master->sendBase;
                sendTime = cur;
            }
        }
        this_thread::sleep_for(milliseconds(std::min((uint)200, master->TimeOut))); // experimental sleep time
    }
}

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
    fs.close();
    
    // after data copying
    beginTask();
}

void Sender::send(const char *buffer, size_t len) {
    busy.lock();
    userBuff = new char[len];
    memcpy(userBuff, buffer, len);
    userDataLen = len;
    
    // after data copying
    beginTask();
}

// the sending thread will loop in this function
void Sender::sending() {
    while (!master->close) {
        this_thread::sleep_for(chrono::milliseconds(50)); // sleep time need to test
        // we are making somehow dangerous assumption here.
        // the listener may need to change the curPtr when
        // packet loss happened
        if (pending) {
//            debug_print("timerBase: %u, sendBase: %u, RTT: %u, timeout: %u\n", timerBase, master->sendBase, master->RTT, master->TimeOut);
            if (master->sendBase - diff >= userDataLen) {
                endTask();
                continue;
            }
            
            // data pending to send, copy data to the buff
            if (resend) {
                // packet loss, request resend by listener
                debug_print("data loss! sendbase: %u\n", master->sendBase);
                curPtr = master->sendBase;
                *((uint*) master->buff) = master->sendBase;
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
                if (master->status == SLOW_START) master->slowStartNum++;
                else if (master->status == CONG_AVOID) master->congestNum++;

                master->setAckBit(0);   // set ack 0
                // if we have any sending error, the usock class
                // will print debug infomation, and we just ignore any
                // sending error here, we let ACK and resend to fix it.
                
                // when timer times up, we need to know whether we have
                // also, we need this to calculate the RTT
                
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
            // not timing only ack packets
            master->setAckBit(0);
        }
    }
}

uint Sender::byteIncWnd() {
    return master->cWnd - (curPtr - master->sendBase);
}
