//
//  TimeoutTimer.cpp
//  ReliableUDP
//
//  Created by Wenzhe Lu on 2018/3/25.
//  Copyright © 2018年 Wenzhe Lu. All rights reserved.
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
