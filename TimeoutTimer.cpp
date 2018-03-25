//
//  TimeoutTimer.cpp
//  ReliableUDP
//

#include "RUDP.hpp"
#include "TimeoutTimer.hpp"

TimeoutTimer::TimeoutTimer(milliseconds timeout_ms, Sender *s, uint start) : Timer(timeout_ms), sender(s), startByte(start) {}

void TimeoutTimer::run() {
    RUDP *master = sender->master;
    
    if (startByte >= master->sendBase) {
        // if larger or equals, it means this packet times out and we haven't
        // receive its ack
        master->status = SLOW_START;
        // TODO: symptons of SLOWSTART
        
        
        sender->resend = true;

    }
}
