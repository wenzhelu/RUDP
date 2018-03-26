//
//  sender.hpp
//  ReliableUDP
//

#ifndef sender_hpp
#define sender_hpp

#include "RUDP.hpp"
#include <mutex>
#include <thread>

// only one sender
class Sender {
private:
    void timing();
    
    void updateTimer();
    
    void endTask();
    
    // short cut
    // calculate how many bytes left in the congestion window to send.
    uint byteIncWnd();
    
public:
    char *userBuff;
    uint userDataLen;
    std::mutex busy;    // if there are other files in transit, this will lock
    bool pending;       // if their is data pending to send
    bool resend;
    uint curPtr;        // real pointer to the databuff, notice the difference to the sendbase
    uint diff;          // the difference of sendBase with the dataBuff, for mapping purpose
    RUDP *master;

    thread *th_timer;
    uint timerBase;     // the send base when the timer is set
    bool timerSet;      // timer set flag
    ms sendTime;        // the time when the timer is set
    
    //    long long packets;
    //    Timer *t;
    //    bool timerDone;
    
    Sender(RUDP*);
    
    // notice that if the sender is busy, the caller is stuck
    void send(const char*);
    
    void send(const char*, size_t);
    
    void sending();
    
};

#endif /* sender_hpp */
