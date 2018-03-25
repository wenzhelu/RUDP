
//
// main.cpp
// Reliable UDP
//

#include <iostream>
#include "include/RUDP.hpp"
#include "include/sender.hpp"

// driver code
int main(int argc, const char **argv) {
    RUDP protocol;
    if (DEBUG_FLAG) {
        protocol.init(30000, "6666", "127.0.0.1", "7777");
        string s = "aaaaaaa";
        protocol.sender->send(s.data(), 7);
        while(1);
        return 1;
    }
    
    if (argc != 4) {
        using namespace std;
        cout << "usage:\n ./rudp local_port remote_ip remote_port" << endl;
    }
    protocol.init(30000, argv[1], argv[2], argv[3]);
    
    // until now the protocol have nothing to do,
    // we have to make it send something
//    string s = "aaaaaaa";
//    protocol.sender->send(s.data(), 7);
    while (1);
    
//    protocol.shutdown();
//    protocol.cleanUp();
    
    return 0;
    // all send tasks done, shutdown
}
