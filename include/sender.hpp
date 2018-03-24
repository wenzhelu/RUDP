//
//  sender.hpp
//  ReliableUDP
//
//  Created by Wenzhe Lu on 2018/3/22.
//  Copyright © 2018年 Wenzhe Lu. All rights reserved.
//

#ifndef sender_hpp
#define sender_hpp

#include "RUDP.hpp"
#include <mutex>

// only one sender
class Sender {
public:
    char *userBuff;
    size_t userDataLen;
    std::mutex busy;    // if there are other files in transit, this will lock
    bool pending;       // if their is data pending to send
    bool resend;
    bool close;         // close flag set by RUDP
    size_t curPtr;      // real pointer to the databuff, notice the difference to the sendbase
    size_t diff;        // the difference of sendBase with the dataBuff, for mapping purpose
    // so that the "real" send base would be RUDP::sendBase - diff
    
    Sender();
    
    // notice that if the sender is busy, the caller is stuck
    void send(char*);
    
    void send(char*, size_t);
    
    void sending();
};

#endif /* sender_hpp */
