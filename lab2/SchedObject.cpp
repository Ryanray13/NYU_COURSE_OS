#include "SchedObject.h"
#include "Simulator.h"

//Scheduler FCFS
void SchedulerFCFS::getProcess()
{
	this->it=this->readyQueue.begin();
	it->durTime=this->timeStamp-it->timeStamp;
}


//Scheduler LCFS
void SchedulerLCFS::getProcess()
{
	this->it=this->readyQueue.end();
	it--;
	it->durTime=this->timeStamp-it->timeStamp;
}

//Schduler SJF
void SchedulerSJF::getProcess()
{
	this->it=this->readyQueue.begin();
	it->durTime=this->timeStamp-it->timeStamp;
}

void SchedulerSJF::addProcess(Process &p)
{
	list<Process>::iterator pit=readyQueue.begin();
	for(;pit!=readyQueue.end();++pit)
	{
		if(p.remTime<pit->remTime)
			break;
	}
	readyQueue.insert(pit,p);
}

//Schduler RoundRobin
SchedulerRR::SchedulerRR(bool v, int quantum):Scheduler(v)
{
	this->quantum=quantum;
}

void SchedulerRR::getProcess()
{
	this->it=this->readyQueue.begin();
	it->durTime=this->timeStamp-it->timeStamp;
}

void SchedulerRR::putEvent(Simulator& sim)
{
	int cb;
	if(it->currentCPUBurst==0)
	{
		cb=this->getRand(sim,it->CPUBurst);
        if(cb<=it->remTime)
		    it->currentCPUBurst=cb;
	    else
		    it->currentCPUBurst=it->remTime;
	}

	cb=it->currentCPUBurst;
	if(this->isVerbose)
		this->printfTrace();
	it->waitingTime += it->durTime;
	this->isRunning=1;

	if(cb<=this->quantum)
	{
		it->durTime=cb;
     	it->remTime -= cb;
    	it->stateTrans=1;
		it->timeStamp=this->timeStamp+cb;
	}
	else
	{
		it->durTime=this->quantum;
     	it->remTime -= this->quantum;
		it->currentCPUBurst =cb - this->quantum;
    	it->stateTrans=2;
		it->timeStamp=this->timeStamp+this->quantum;
	}

	sim.putEvent(*it);
	this->readyQueue.erase(it);
}

