// Listener.cpp

#include "include/RUDP.hpp"
#include "include/Listener.hpp"
#include "include/UDPSock.hpp"

void Listener::recAns()
{
	printf("recAns is invoked\n");
	while(1)
	{
		if(!randomdrop(0.0))
		{
		
			int recbits;
			char rec[1472];
			char databuf[1460];
			bool databit=false;
			bool ackbit=false;
	 		recbits=master->sock->read(rec,1472);
//<<<<<<< HEAD
//            printf("%d bytes received\n",recbits);
////            debug_print("%s", rec);
//            getTimeout(*(int*)rec+recbits);
//=======
			master->sock->printPacket(true,rec,(uint)recbits);
		//	printf("%d bytes received\n",recbits);
		//	printf("sequence num: %u \n",*(uint*)rec);
			getTimeout(*(uint*)rec+recbits);
			if(rec[Listener::control]>>7)
				databit=true;
			if((rec[Listener::control]>>6)&1)
				ackbit=true;
			if(ackbit)
			{	
		//		printf("ack exist\n");
				this->update(*(uint*)(rec+4));
			}
			if(databit)
			{
		//		printf("data exist\n");	
				*(uint*)(master->buff+4)=*(uint*)rec+recbits;
				master->buff[Listener::control]|=1<<6;
				strncpy(databuf,rec+12,1460);
		//		printf("%s\n\n",databuf);
			}
		}
	}		
}
void Listener::update(unsigned int ack)
{
	if(master->status==statusEnum::SLOW_START)
	{
		if(ack==master->sendBase)
		{
			this->duplicateACK+=1;
			if(this->duplicateACK==3)
				this->fastRecovery();
		}
		else
		{
			this->index(ack);
			if(master->cWnd>=master->throughput)
				master->status=statusEnum::CONG_AVOID;
		}
	}
	else if(master->status==statusEnum::FAST_REC)
	{
		if(ack==master->sendBase)
			;
		else
		{
			
			master->status=statusEnum::CONG_AVOID;
			this->linear(ack);
		}
		
    	}	
	else
	{
		if(ack==master->sendBase)
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
	master->throughput=master->cWnd/2;
	master->cWnd=master->throughput;
	master->status=statusEnum::FAST_REC;
}
void Listener::index(unsigned int ack)
{
	master->cWnd+=(ack-master->sendBase);
	if(master->cWnd>master->winSize)
		master->cWnd=master->winSize;
	master->sendBase=ack;
}
void Listener::linear(unsigned int ack)
{
	this->ACKcwnd+=(ack-master->sendBase);
	master->cWnd+=(ACKcwnd*master->PACKET_SIZE)/master->cWnd;
	if(master->cWnd>master->winSize)
		master->cWnd=master->winSize;
	this->ACKcwnd=0;
	master->sendBase=ack;
}
void Listener::getTimeout(int ack)
{
	tp end=std::chrono::system_clock::now();
	tp start;
	if(master->startTimes.find(ack)==master->startTimes.end())
		printf("ack error\n");
	else
	{
		start=master->startTimes.at(ack);
		master->startTimes.erase(master->startTimes.begin(),++master->startTimes.find(ack));
	}
	this->sRTT=std::chrono::milliseconds((end-start).count());
	int u,phi,delta;
	u=1;
	phi=4;
	delta=2;
	this->eRTT=this->eRTT+std::chrono::milliseconds((this->sRTT.count()-this->eRTT.count())/delta);
	this->deviation=this->deviation+std::chrono::milliseconds(abs(this->sRTT.count()-this->eRTT.count())/delta-this->deviation.count());
	master->TimeOut=u*this->eRTT.count()+phi*this->deviation.count();
	master->RTT=this->eRTT.count();
}

bool Listener::randomdrop(double n)
{
	return (rand()%100)<(100*n);
}
