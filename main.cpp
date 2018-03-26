
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
#ifndef LISTEN
    // client here
//    if (DEBUG_FLAG) {
    protocol.init(100000000, "6666", "144.202.116.155", "7777");
//        string s = "aaaaaaa";
//        protocol.sender->send("/Users/wenzhelu/doc/SP2018/hw2.tar.gz");
//        protocol.sender->send("/Users/wenzhelu/vimrc_bak");
//    protocol.sender->send("/Users/wenzhelu/doc/manuals/bash.pdf");
    while(1);
    return 1;
//    }
    
#else
    // server here
    protocol.init(100000000, "7777", "149.165.157.75", "6666");
    //        string s = "aaaaaaa";
    //        protocol.sender->send("/Users/wenzhelu/doc/SP2018/hw2.tar.gz");
    //        protocol.sender->send("/Users/wenzhelu/vimrc_bak");
    protocol.sender->send("/Users/wenzhelu/doc/manuals/bash.pdf");
    while(1);
    return 1;
#endif
    
//    if (argc != 4) {
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
