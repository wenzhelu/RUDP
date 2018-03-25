//
//  TimeoutTimer.hpp
//  ReliableUDP
//

#ifndef TimeoutTimer_h
#define TimeoutTimer_h

#include "Timer.hpp"
#include "sender.hpp"

class TimeoutTimer : public Timer {
private:
    Sender *sender;
    uint startByte;    // for mapping
    
public:
    TimeoutTimer(milliseconds timeout_ms, Sender *s, uint startByte);
    
    void run();
};


#endif /* TimeoutTimer_h */
