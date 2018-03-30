
//
// main.cpp
// Reliable UDP
//

#include <iostream>
#include "include/RUDP.hpp"
#include "include/sender.hpp"


int main(int argc, const char **argv) {
    RUDP protocol;
//    protocol.sender->send("/Users/wenzhelu/doc/SP2018/hw2.tar.gz");
//    protocol.sender->send("/Users/wenzhelu/vimrc_bak");
//    protocol.sender->send("/Users/wenzhelu/doc/manuals/bash.pdf");
    protocol.init(100000000, "6666", "144.202.116.155", "6666");
    protocol.sender->send("./bash.pdf");
    while(1);
    return 1;
//#endif
    
//    
//        using namespace std;
//        cout << "usage:\n ./rudp local_port remote_ip remote_port" << endl;
//    }
//    protocol.init(30000, argv[1], argv[2], argv[3]);
//
//    // until now the protocol have nothing to do,
//    // we have to make it send something
////    string s = "aaaaaaa";
////    protocol.sender->send(s.data(), 7);
//    while (1);
//
////    protocol.shutdown();
////    protocol.cleanUp();
//
//    return 0;
    // all send tasks done, shutdown
}
