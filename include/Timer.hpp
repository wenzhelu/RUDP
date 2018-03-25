//
//  Timer.hpp
//  ReliableUDP
//

#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;
using std::thread;

class Timer {
private:
    microseconds ms;
    
public:
    Timer(microseconds ms) {
        this->ms = ms;
        thread([this]{
            start();
        });
    }
    
    void start() {
        std::this_thread::sleep_for(ms);
        run();
    }
    
    virtual void run() {
        std::cout << "running base class" << std::endl;
    }
};

#endif /* Timer_hpp */
