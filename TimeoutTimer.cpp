//
//  TimeoutTimer.cpp
//  ReliableUDP
//

#include "include/RUDP.hpp"
#include "include/TimeoutTimer.hpp"

TimeoutTimer::TimeoutTimer(milliseconds timeout_ms, Sender *s, uint start) : Timer(timeout_ms), sender(s), startByte(start) {}

void TimeoutTimer::run() {
    RUDP *master = sender->master;
    if (startByte >= master->sendBase) {
        // if larger or equals, it means this packet times out and we haven't
        // receive its ack
        master->status = SLOW_START;
        master->throughput = master->cWnd >> 1;
        master->cWnd = RUDP::PACKET_SIZE;
        sender->resend = true;
//        sender->timerDone = true;
    }
}
