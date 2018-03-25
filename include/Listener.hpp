#ifndef Listener_hpp
#define Listener_hpp

class Listener{
public:
	int duplicateACK;
	unsigned int ACKcwnd;
	ms eRTT;
	ms sRTT;
	ms deviation;
	static const int control=8;

	Listener(){duplicateACK=ACKcwnd=0;};
	void recAns();
	void update(unsigned int sendbase);
	void fastRecovery();
	void index(unsigned int sendbase);
	void linear(unsigned int sendbase);
	void getTimeout(int ack);
	bool randomdrop(double probility);
};

#endif
	 
