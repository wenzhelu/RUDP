//
//  sender.hpp
//  ReliableUDP
//

#ifndef sender_hpp
#define sender_hpp

#include "RUDP.hpp"
#include "TimeoutTimer.hpp"
#include <mutex>
#include <list>

// only one sender
class Sender {
public:
    char *userBuff;
    uint userDataLen;
    std::mutex busy;    // if there are other files in transit, this will lock
    bool pending;       // if their is data pending to send
    bool resend;
    uint curPtr;      // real pointer to the databuff, notice the difference to the sendbase
    uint diff;        // the difference of sendBase with the dataBuff, for mapping purpose
    RUDP *master;
    long long packets;

    // so that the "real" send base would be RUDP::sendBase - diff
    
    Sender(RUDP*);
    
    // notice that if the sender is busy, the caller is stuck
    void send(const char*);
    
    void send(const char*, size_t);
    
    void sending();
    
private:
    // short cut
    // calculate how many bytes left in the congestion window to send.
    uint byteIncWnd();
};

#endif /* sender_hpp */
