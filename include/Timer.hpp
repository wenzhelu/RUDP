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
    }
    
    void count() {
        std::this_thread::sleep_for(ms);
        run();
    }
    
    void start() {
        thread t(&Timer::count, this);
    }
    
    virtual void run() {
        std::cout << "running base class is not allowed, do nothing" << std::endl;
    }
};

#endif /* Timer_hpp */
