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
    
    void endTask();
    
    // short cut
    // calculate how many bytes left in the congestion window to send.
    uint byteIncWnd();
    
    // get two time points difference in milliseconds
    // the first parameter is begin, the second one is end
    uint getDurMs(const tp&, const tp&);
    
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
    tp sendTime;        // the time when the timer is set
    
    Sender(RUDP*);
    
    // notice that if the sender is busy, the caller is stuck
    void send(const char*);
    
    void send(const char*, size_t);
    
    void sending();
    
};

#endif /* sender_hpp */
