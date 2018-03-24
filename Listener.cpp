#include "Listener.hpp"

void Listener::recAns()
{
	if(!randomdrop(1))
	{
		
		int recbits;
		char rec[1472];
		char databuf[1460];
		bool databit=false;
		bool ackbit=false;
		getTimeout();
		if(rec[Listener::control]>>7)
		databit=true;
		if((rec[Listener::control]>>6)&1)
			ackbit=true;
		if(ackbit)
			this->update(*(int*)(rec+4));
		if(databit)
		{	
			*(int*)(RUDP::buff+4)=*(int*)rec+recbits;
			RUDP::buff[Listener::control]|=1<<6;
		}
	}	
}
void Listener::update(unsigned int ack)
{
	if(RUDP::status==statusEnum::SLOW_START)
	{
		if(ack<RUDP::sendBase)
		{
			this->duplicateACK+=1;
			if(this->duplicateACK==3)
				this->fastRecovery();
		}
		else
		{
			this->index(ack);
			if(RUDP::cWnd>=RUDP::throughput)
				RUDP::status=statusEnum::CONG_AVOID;
		}
	}
	else if(RUDP::status==statusEnum::FAST_REC)
	{
		if(ack<RUDP::sendBase)
			;
		else
		{
			
			RUDP::status=statusEnum::CONG_AVOID;
			this->linear(ack);
		}
		
    	}	
	else
	{
		if(ack<RUDP::sendBase)
		{
			this->duplicateACK+=1;
			if(this->duplicateACK==3)
				this->fastRecovery();
		}
		else
		{
			this->linear(ack);
		}	
	}
}
	
void Listener::fastRecovery()
{
	this->duplicateACK=0;
	RUDP::throughput=RUDP::cWnd/2;
	RUDP::cWnd=RUDP::throughput;
	RUDP::status=statusEnum::FAST_REC;
}
void Listener::index(unsigned int ack)
{
	RUDP::cWnd+=(ack-RUDP::sendBase);
	if(RUDP::cWnd>RUDP::winSize)
		RUDP::cWnd==RUDP::winSize;
	RUDP::sendBase=ack;
}
void Listener::linear(unsigned int ack)
{
	this->ACKcwnd+=(ack-RUDP::sendBase);
	RUDP::cWnd+=(ACKcwnd*RUDP::PACKET_SIZE)/RUDP::cWnd;
	if(RUDP::cWnd>RUDP::winSize)
		RUDP::cWnd=RUDP::winSize;
	this->ACKcwnd=0;
	RUDP::sendBase=ack;
}
void Listener::getTimeout()
{
	ms end;
	int u,phi,delta;
	u=1;
	phi=4;
	delta=2;
	this->eRTT=this->eRTT+std::chrono::milliseconds((this->sRTT.count()-this->eRTT.count())/delta);
	this->deviation=this->deviation+std::chrono::milliseconds(abs(this->sRTT.count()-this->eRTT.count())/delta-this->deviation.count());
	RUDP::TimeOut=u*this->eRTT.count()+phi*this->deviation.count();
	RUDP::RTT=this->eRTT.count();
}

bool randomdrop(double n)
{
	return (rand()%100)<=(99*n);
}
