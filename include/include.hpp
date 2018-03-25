#ifndef include_hpp
#define include_hpp

#include <cstdlib>
#include <chrono>
#include <map>
#include <thread>
#include <cmath>
#include <string>
#include <cstring>
#include <stdio.h>


using namespace std;
using namespace chrono;

typedef unsigned int uint;
typedef std::chrono::milliseconds ms;
typedef std::chrono::system_clock::time_point tp;
typedef enum status{
	SLOW_START,
	FAST_REC,
	CONG_AVOID,
}statusEnum;

#endif
