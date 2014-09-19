#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Simulator.h"
using namespace std;

extern string Scheduler_Type[];

string transition[4]={
	"READY -> READY",
	"RUNNG -> BLOCK",
	"RUNNG -> READY",
	"BLOCK -> READY"
};

enum State_Transition{
	Ready_to_Ready=0,
    Running_to_Blocked=1,
	Running_to_Ready=2,
	Blocked_to_Ready=3
};

Simulator::Simulator( int type,bool v, int quan)
{
	this->isVerbose=v;
	this->quantum=quan;
	this->schedType=type;
	this->randofs=-1;
	this->blockCount=0;
	this->totalIO=0;
	if(type==0)
		this->sched=new SchedulerFCFS(v);
	else if(type==1)
		this->sched=new SchedulerRR(v,quan);
	else if(type==2)
		this->sched=new SchedulerLCFS(v);
	else
		this->sched=new SchedulerSJF(v);
}
	
Simulator::~Simulator()
{
	delete this->sched;
}

void Simulator::printTrace(int io)
{
	int id=it->pid;
	int ptime=it->timeStamp;
	int dtime=it->durTime;
	int state=it->stateTrans;

	cout<<endl<<"==> "<<ptime<<" "<<id<<" ts="<<ptime-dtime<<" ";
	if(state==1)
		cout<<"BLOCK";
	else if(state==2)
		cout<<"PREEMPT";
	else
		cout<<"READY";
	cout<<"  dur="<<dtime<<endl;
	
	if(state==1 && it->remTime==0)
	{
		cout<<"==> T("<<id<<"): Done"<<endl;
		return ;
	}
	
	cout<<"T("<<id<<":"<<ptime<<"): "<<transition[state];
    if(state==1)
		cout<<"  ib="<<io<<" rem="<<it->remTime;
	else if(state==2)
		cout<<"  cb="<<it->currentCPUBurst<<" rem="<<it->remTime;

	cout<<endl;
}
   
void Simulator::printResults()
{
	double CPUTime=0.0,turnTime=0.0,waitTime=0.0;
	int finTime=0;
	int size=processList.size();
	multiset<Process>::iterator it;
	cout<<Scheduler_Type[schedType];
	if(schedType==1)
		cout<<" "<<this->quantum;
	cout<<endl;
	for(it=processList.begin();it!=processList.end();++it)
	{
		cout.fill('0');
		cout<<setw(4)<<it->pid<<": ";
		cout.fill(' ');
		cout<<setw(4)<<it->arrivalTime<<" "<<setw(4)<<it->totalCPUTime<<" "<<setw(4)<<it->CPUBurst
			<<" "<<setw(4)<<it->IOBurst<<" | "<<setw(4)<<it->finishTime<<" "<<setw(4)<<it->finishTime-it->arrivalTime
			<<" "<<setw(4)<<it->IOTime<<" "<<setw(4)<<it->waitingTime<<endl;
		CPUTime += it->totalCPUTime;
		turnTime += it->finishTime-it->arrivalTime;
		waitTime += it->waitingTime;
		if(finTime<it->finishTime)
			finTime=it->finishTime;
	}

	cout<<"SUM: "<<finTime;
	cout<<setiosflags(ios_base::fixed);
	cout<<" "<<setprecision(2)<<CPUTime*100/finTime<<" "<<setprecision(2)<<(double)totalIO*100/finTime<<" "<<setprecision(2)<<turnTime/size
		<<" "<<setprecision(2)<<waitTime/size<<" "<<setprecision(3)<<(double)size*100/finTime<<endl;
}

int Simulator::readInput(char* path)
{
	this->inFile.open(path,ios_base::binary);
	if(!inFile){
		cout<<"Error! Cannot open the input file!"<<endl;
		inFile.close();	
		return 0;
	}
	Process *p;
	int id=0;
	int at=0,tc=0,cb=0,io=0;
	inFile>>at;
	while(!inFile.eof())
	{
		inFile>>tc>>cb>>io;
		p=new Process(id,at,tc,cb,io);
		p->timeStamp=at;
		p->stateTrans=0;
		p->durTime=0;
		this->putEvent(*p);
		id++;
		delete p;
		inFile>>at;
	}

	inFile.clear();
	inFile.close();
	return 1;
}

int Simulator::readRandom(char* path)
{
	this->inFile.open(path,ios_base::binary);
	if(!inFile){
		cout<<"Error! Cannot open the random number file!"<<endl;
		inFile.close();
		return 0;
	}
    inFile>>this->randCount;
	int temp;
	while(!inFile.eof())
	{
		inFile>>temp;
		this->randNum.push_back(temp);
	}
	inFile.clear();
	inFile.close();
	return 1;
}

	
void Simulator::invokeSched()
{
	sched->setTimeStamp(this->timeStamp);
	sched->getProcess();
	sched->putEvent(*this);
}
	
	
int Simulator::getRand(int burst)
{
	randofs=(randofs+1)%randCount;
	return 1+randNum[randofs]%burst;
}

void Simulator::getEvent()
{
	it=eventQueue.begin();
}

void Simulator::putEvent(Process& p)
{
	eventQueue.insert(p);
}
void Simulator::dealEvent()
{
	Process p;
	int eventCount;
	int i;
	int state=-1;
	while(!eventQueue.empty())
	{
		this->getEvent();
		this->timeStamp=it->timeStamp;
		eventCount=1;
		do{
			p=(*it);
            state=p.stateTrans;
			if(state==1 && p.remTime!=0)
				p.currentIOBurst=this->getRand(p.IOBurst);
			
			if(this->isVerbose)                                    //verbose print
			{
				if(eventCount>1 && !sched->getStatus())
					cout<<"   delay scheduler"<<endl;
				this->printTrace(p.currentIOBurst);
			}

			if(state==1)
			{
				sched->setStatus(0);                                //change the status of cpu when process is blocked
				if(p.remTime!=0)
				{
					if(blockCount==0)                              //if this process is the first that begins the IO store the timeStamp
						this->IObegin=timeStamp;
					this->blockCount++; 
					p.IOTime += p.currentIOBurst;
					p.durTime = p.currentIOBurst;
					p.timeStamp += p.currentIOBurst;
					p.stateTrans=3;
					p.currentCPUBurst=0;
					this->putEvent(p);                            //add event to eventQueue.
				}
				else
				{
					p.finishTime=this->timeStamp;
					p.timeStamp=p.pid;
					this->putProcess(p);                          //add process to processList when finished
				}
			}
			else if(state==2)                                            //change the status of cpu when preempted
			{
				sched->setStatus(0);
				sched->addProcess(p);
			}
			else if(state==3)
			{
				this->blockCount--;
				if(blockCount==0)
					this->totalIO += timeStamp-this->IObegin;
				sched->addProcess(p);
			}
			else
				sched->addProcess(p);                             //add process to readyQueue.
  			eventQueue.erase(it);
			if(eventQueue.empty())
				break;
			this->getEvent();
			eventCount++;
		}while(this->timeStamp==it->timeStamp);

		//when cpu is not runing and ready queue is not empty, invoke the scheduler.
		if(!sched->getStatus() && !sched->checkQueue())
			this->invokeSched();
	}
}

void Simulator::putProcess(Process& p)
{
	this->processList.insert(p);
}