#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <list>
#include "Process.h"

class Simulator;

class Scheduler
{
protected:
	bool isRunning;                                     //indicate whether cpu is running
	bool isVerbose;                                     //indicate whether needs a verbose print
    int timeStamp;                                      //indicate time when scheduler is called
	std::list<Process> readyQueue;                      
	std::list<Process>::iterator it; 

public:
	Scheduler(bool v);
	virtual ~Scheduler(){}
	bool getStatus();                                    //check whether cpu is running
	void setStatus(bool s);
	void setTimeStamp(int t);                                                                 
	void printfTrace();                                  //when verbose print trace
	virtual void addProcess(Process& p);                 //for simulator to add process to ready queue
	int getRand(Simulator &sim, int burst);              //generate CPU burst
	bool checkQueue();                                   //check whether the queue is empty
    virtual void getProcess()=0;                         //select a process from readyQueue
	virtual void putEvent( Simulator& sim);              //put a event to eventQueue
};


#endif