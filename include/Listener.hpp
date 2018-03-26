#ifndef Listener_hpp
#define Listener_hpp

#include "RUDP.hpp"

class Listener{
public:
	int duplicateACK;
	unsigned int ACKcwnd;
	ms eRTT;
	ms sRTT;
	ms deviation;
    RUDP *master;
	static const int control=8;
    uint totalrec;

    Listener(RUDP *m){duplicateACK=ACKcwnd=totalrec=0; master = m;};
	void recAns();
	void update(unsigned int sendbase);
	void fastRecovery();
	void index(unsigned int sendbase);
	void linear(unsigned int sendbase);
	void getTimeout(int ack);
	bool randomdrop(double probility);
};

#endif
	 
