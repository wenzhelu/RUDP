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
    thread *t;
    
public:
    Timer(microseconds ms) {
        this->ms = ms;
        t = new thread(&Timer::count, this);
    }
    
    void count() {
        std::this_thread::sleep_for(ms);
        run();
    }
    
    virtual void run() {
        std::cout << "running base class is not allowed, do nothing" << std::endl;
    }
    
    ~Timer() {
        delete t;
        delete this;
        printf("[DEBUG] Timer susscessful freed\n");
    }
};

#endif /* Timer_hpp */
