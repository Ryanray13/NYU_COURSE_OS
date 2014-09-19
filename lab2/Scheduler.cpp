#include <iostream>
#include "Scheduler.h"
#include "Simulator.h"
using namespace std;


Scheduler::Scheduler(bool v)
{
	this->isVerbose=v;
	this->isRunning=0;
}

bool Scheduler::getStatus()
{
   return this->isRunning;
}

void Scheduler::setStatus(bool s)
{
    this->isRunning=s;
}

void Scheduler::setTimeStamp(int t)
{
	this->timeStamp=t;
}


int Scheduler::getRand(Simulator &sim, int burst)
{
     return sim.getRand(burst);
}

void Scheduler::addProcess(Process& p)
{
	this->readyQueue.push_back(p);
}

void Scheduler::printfTrace()
{
	int id=it->pid;
	int ptime=it->timeStamp;
	cout<<endl<<"==> "<<this->timeStamp<<" "<<id<<" ts="<<ptime<<" RUNNG  dur="<<it->durTime<<endl;
	cout<<"T("<<id<<":"<<this->timeStamp<<"): READY -> RUNNG"<<"  cb="<<it->currentCPUBurst<<" rem="<<it->remTime<<endl;
}


void Scheduler::putEvent( Simulator& sim)
{
	int cb=this->getRand(sim,it->CPUBurst);
	if(cb<=it->remTime)
		it->currentCPUBurst=cb;
	else
		it->currentCPUBurst=it->remTime;

	cb=it->currentCPUBurst;
	if(this->isVerbose)
		this->printfTrace();
	it->waitingTime += it->durTime;
	this->isRunning=1;

	it->durTime=cb;
	it->remTime -= cb;
	it->stateTrans=1;
	it->timeStamp=this->timeStamp+cb;
	sim.putEvent(*it);
	this->readyQueue.erase(it);
}


bool Scheduler::checkQueue() 
{
	return this->readyQueue.empty();
}
