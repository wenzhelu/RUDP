#ifndef Listener_hpp
#define Listener_hpp

#include "RUDP.hpp"

class Listener{
public:
	int duplicateACK = 0;
	unsigned int ACKcwnd = 0;
	ms eRTT;
	ms sRTT;
	ms deviation;
	static const int control=8;

	void recAns();
	void update(unsigned int sendbase);
	void fastRecovery();
	void index(unsigned int sendbase);
	void linear(unsigned int sendbase);
	void getTimeout();
	bool randomdrop(double probility);
};

#endif
	 
