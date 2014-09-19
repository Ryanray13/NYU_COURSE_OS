#include "Process.h"


Process::Process(int pid,int at, int tc, int cb, int io)
{
	this->pid=pid;
	this->arrivalTime=at;
	this->totalCPUTime=tc;
	this->CPUBurst=cb;
	this->IOBurst=io;
	this->IOTime=0;
	this->waitingTime=0;
	this->remTime=tc;
	this->currentCPUBurst=0;
	this->currentIOBurst=0;
}

bool Process::operator < (const Process& p) const            //overload operator '<'
{
	return this->pid<p.pid;
}

