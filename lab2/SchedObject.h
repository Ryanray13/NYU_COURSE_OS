#ifndef SCHEDOBJECT_H
#define SCHEDOBJECT_H

#include "Scheduler.h"

//Scheduler FCFS
class SchedulerFCFS : public Scheduler
{
public:
	SchedulerFCFS(bool v):Scheduler(v){}
	void getProcess();
};

//Scheduler LCFS
class SchedulerLCFS : public Scheduler
{
public:
	SchedulerLCFS(bool v):Scheduler(v){}
	void getProcess();
};


//Scheduler SJF
class SchedulerSJF : public Scheduler
{
public:
	SchedulerSJF(bool v):Scheduler(v){}
	void getProcess();
	void addProcess(Process &p);
};

//Scheduler RR
class SchedulerRR : public Scheduler
{
private:
	int quantum;
public:
	SchedulerRR(bool v, int quantum);
	void getProcess();
	void putEvent( Simulator& sim);
};


#endif