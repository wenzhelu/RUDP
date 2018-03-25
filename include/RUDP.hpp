//
//  RUDP.hpp
//  ReliableUDP
//

#ifndef RUDP_hpp
#define RUDP_hpp

typedef unsigned int uint;

#include <cstdlib>
#include <chrono>
#include <map>
#include <cmath>
#include <thread>
#include <string>
#include <cstdio>
#include <cstring>

#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif
#define debug_print(fmt, ...) \
do { if (DEBUG_FLAG) { \
fprintf(stderr, "[DEBUG] ");\
fprintf(stderr, fmt, __VA_ARGS__); } \
} while (0)

using namespace std;
using namespace chrono;    // for milliseconds

class Sender;
class Listener;
class Usock;

typedef enum status {
    SLOW_START,
    FAST_REC,
    CONG_AVOID,
} statusEnum;

typedef std::chrono::milliseconds ms;
typedef std::chrono::system_clock::time_point tp;	

// class for the protocol
class RUDP {
    
private:
    
public:
    static const short PACKET_SIZE = 1460;  // max packet size in byte - only data here
    static const short HEADER_LEN = 12;     // packet header length
    
    RUDP() {}
    
    uint sendBase;       // send base
    uint winSize;        // Note that this parameter is set by administrator
    uint throughput;     // set when first timeout happend, defaultly INT_MAX
    statusEnum status;   //
    uint cWnd;           // Congestion Window
    uint RTT;            // updated by listener
    uint TimeOut;        // updated by listener
    map<uint, tp> startTimes;
    char buff[PACKET_SIZE + HEADER_LEN];     // 1500 - 8(UDP) - 20(IP)
    Sender *sender;
    Listener *listener;
    bool close;
    Usock *sock;
    thread *th_listener;
    thread *th_sender;
//    static uint timeout;      // we decide to let timer to set RTT
    
    // in charge of setting default parameters and
    // intializing sending and listener
    void init(uint window, const char *port, const char *remoteIp, const char *remotePort);
    
    void setDataBit(bool tf);
    
    void setAckBit(bool tf);
    
    bool testDataBit();
    
    bool testAckBit();
    
    void shutdown();
    
    void cleanUp();
    
};

#endif /* RUDP_hpp */
