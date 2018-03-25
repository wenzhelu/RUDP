
//
// main.cpp
// Reliable UDP
//

#include "include/RUDP.hpp"
#include "include/sender.hpp"

// driver code
int main() {
    RUDP protocol;
    protocol.init(30000, "7777", "xxx", "7777");
    
    // until now the protocol have nothing to do,
    // we have to make it send something
    string s = "aaaaaaa";
    protocol.sender->send(s.data(), 7);
    while (1);
    
//    protocol.shutdown();
//    protocol.cleanUp();
    
    return 0;
    // all send tasks done, shutdown
}
