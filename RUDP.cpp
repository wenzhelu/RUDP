//
//  RUDP.cpp
//  ReliableUDP
//

#include "include/RUDP.hpp"
#include "include/UDPSock.hpp"
#include "include/Listener.hpp"
#include "include/sender.hpp"

void RUDP::setDataBit(bool tf){
    if(tf) buff[8] |= 1 << 7;
    else buff[8] &= ~(1 << 7);
}

void RUDP::setAckBit(bool tf){
    if(tf) buff[8] |= 1 << 6;
    else buff[8] &= ~(1 << 6);
}

void RUDP::setFinBit(bool tf){
    if(tf) buff[8] |= 1 << 5;
    else buff[8] &= ~(1 << 5);
}

bool RUDP::testDataBit() {
    return (buff[8] >> 7) & 1;
}

bool RUDP::testAckBit() {
    return (buff[8] >> 6) & 1;
}

bool RUDP::testFinBit() {
    return (buff[8] >> 5) & 1;
}

// in charge of setting default parameters and
// intializing sending and listener
void RUDP::init(uint window, const char *port, const char *remoteIp, const char *remotePort) {
    srand(time(nullptr));   // set random seed
    memset(buff, 0, PACKET_SIZE + HEADER_LEN);
    this->sendBase = 0;
    // make the sequence number in the buffer the same with send base
    *(uint *)(this->buff) = this->sendBase;
    this->winSize = window;
    this->status = SLOW_START;
    this->cWnd = PACKET_SIZE;
    this->close = false;
    this->RTT = 500;
    this->TimeOut = 3000;
    this->throughput = 100000000;
    this->congestNum = 0;
    this->slowStartNum = 0;
    
    this->sock = new Usock();
    this->sock->init(port, remoteIp, remotePort);
    this->sender = new Sender(this);
    this->listener = new Listener(this);
    
    th_sender = new thread(&Sender::sending, sender);
    th_listener = new thread(&Listener::recAns, listener);
}

void RUDP::shutdown() {
    // stop protocol running
    close = true;
    th_sender->join();
    th_listener->join();
}

void RUDP::cleanUp() {
    // release resources like socket and heap memory
    sock->closeSock();
    delete th_listener;
    delete th_sender;
    delete sender;
    delete listener;
}
